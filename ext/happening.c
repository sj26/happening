#include <event.h>
#include <ruby.h>

VALUE Happening = Qnil;

struct event_base *happening_event_base;

typedef struct {
    VALUE proc;
} happening_cb_info;

static void happening_cb(int fd, short events, void *arg) {
    happening_cb_info *info = (happening_cb_info *)arg;
    
    rb_proc_call(info->proc, rb_ary_new());
    
    free(info);
}

// Schedule a block to run next
static VALUE cHappening_next(VALUE self) {
    rb_need_block();

    happening_cb_info *info = malloc(sizeof(happening_cb_info));
    info->proc = rb_block_proc();

    int result = event_base_once(happening_event_base, -1, EV_TIMEOUT, happening_cb, (void *)info, NULL);
    if (result != 0)
        rb_raise(rb_eStandardError, "failed to schedule event");

    return Qtrue;
}

// Schedule a block to run after a certain duration
static VALUE cHappening_in(VALUE self, VALUE in) {
    struct timeval tv;
    tv.tv_sec = NUM2LONG(in);
    tv.tv_usec = 0;
    
    rb_need_block();
    
    happening_cb_info *info = malloc(sizeof(happening_cb_info));
    info->proc = rb_block_proc();
    
    int result = event_base_once(happening_event_base, -1, EV_TIMEOUT, happening_cb, (void *)info, &tv);
    if (result != 0)
        rb_raise(rb_eStandardError, "failed to schedule event");
    
    return Qtrue;
}

// Schedule a block to run at a certain date/time
static VALUE cHappening_at(VALUE self, VALUE when) {
    // TODO: Convert Date and Time to numeric timestamp
    // TODO: Check it's a future date/time
    struct timeval tv;
    tv.tv_sec = NUM2LONG(when) - (long int)time(NULL);
    tv.tv_usec = 0;
    
    if (tv.tv_sec < 0)
	    rb_raise(rb_eArgError, "tried to schedule an event in the past");
	    
    rb_need_block();
    
    happening_cb_info *info = malloc(sizeof(happening_cb_info));
    info->proc = rb_block_proc();
    
    int result = event_base_once(happening_event_base, -1, EV_TIMEOUT, happening_cb, (void *)info, &tv);
    if (result != 0)
        rb_raise(rb_eStandardError, "failed to schedule event");
    
    return Qtrue;
}

// Run event loop in current thread
// Schedule a block to run, if given
static VALUE cHappening_run(VALUE self) {
    if (rb_block_given_p()) {
        happening_cb_info *info = malloc(sizeof(happening_cb_info));
        info->proc = rb_block_proc();
        if (event_base_once(happening_event_base, -1, EV_TIMEOUT, happening_cb, (void *)info, NULL))
            rb_raise(rb_eStandardError, "failed to schedule event");
    }
    
    int result = event_base_loop(happening_event_base, 0);
    if (result != 0 && result != 1) {
        rb_raise(rb_eStandardError, "failed to start event loop");
    }
}

// Starts the event loop and returns immediately
// Schedule a block to run first, if given
static VALUE cHappening_start(VALUE self) {
    if (rb_block_given_p()) {
        happening_cb_info *info = malloc(sizeof(happening_cb_info));
        info->proc = rb_block_proc();
        if (event_base_once(happening_event_base, -1, EV_TIMEOUT, happening_cb, (void *)info, NULL))
            rb_raise(rb_eStandardError, "failed to schedule event");
    }
    
    // TODO: Run event loop in a seperate thread
}

// Stops the event loop
static VALUE cHappening_stop(VALUE self) {
    // TODO: Check return value
    event_base_loopexit(happening_event_base, NULL);
}

void Init_happening() {
    happening_event_base = event_base_new();

    Happening = rb_define_module("Happening");
    
    rb_define_module_function(Happening, "next", cHappening_next, 0);
    rb_define_module_function(Happening, "in", cHappening_in, 1);
    rb_define_module_function(Happening, "at", cHappening_at, 1);
    rb_define_module_function(Happening, "start", cHappening_start, 0);
    rb_define_module_function(Happening, "run", cHappening_run, 0);
    rb_define_module_function(Happening, "stop", cHappening_stop, 0);
}

// XXX: How do we de-init and free the event_base?
