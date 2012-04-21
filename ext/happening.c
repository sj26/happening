#include <ev.h>
#include <ruby.h>

VALUE Happening = Qnil;

struct ev_loop *happening_loop;

typedef struct {
    VALUE proc;
} happening_cb_info;

static void happening_cb(int fd, short events, void *arg) {
    happening_cb_info *info = (happening_cb_info *)arg;
    
    rb_proc_call(info->proc, rb_ary_new());
    
    free(info);
}

// Schedule a block to run next
static VALUE Happening_next(VALUE self) {
    rb_need_block();

    happening_cb_info *info = malloc(sizeof(happening_cb_info));
    info->proc = rb_block_proc();

    int result = event_base_once(happening_event_base, -1, EV_TIMEOUT, happening_cb, (void *)info, NULL);
    if (result != 0)
        rb_raise(rb_eStandardError, "failed to schedule event");

    return Qtrue;
}

// Schedule a block to run after a certain duration
static VALUE Happening_in(VALUE self, VALUE in) {
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
static VALUE Happening_at(VALUE self, VALUE when) {
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

// Schedule a block to run when an IO event happens.
static VALUE Happening_when(VALUE self, VALUE subject, VALUE event_type) {
    struct event *event;
    
    int result = event_base_set(happening_event_base, event);
    if (result != 0)
        rb_raise(rb_eStandardError, "failed to schedule event");
    
    return Qtrue;
}

// Run event loop in current thread
// Schedule a block to run, if given
static VALUE Happening_run(VALUE self) {
    if (rb_block_given_p()) {
        happening_cb_info *info = malloc(sizeof(happening_cb_info));
        info->proc = rb_block_proc();
        if (event_base_once(happening_event_base, -1, EV_TIMEOUT, happening_cb, (void *)info, NULL))
            rb_raise(rb_eStandardError, "failed to schedule event");
    }
    
    int result = ev_run(happening_loop, 0);
    if (result != 0 && result != 1) {
        rb_raise(rb_eStandardError, "failed to start event loop");
    }
}

// Starts the event loop and returns immediately
// Schedule a block to run first, if given
static VALUE Happening_start(VALUE self) {
    if (rb_block_given_p()) {
        happening_cb_info *info = malloc(sizeof(happening_cb_info));
        info->proc = rb_block_proc();
        if (event_base_once(happening_event_base, -1, EV_TIMEOUT, happening_cb, (void *)info, NULL))
            rb_raise(rb_eStandardError, "failed to schedule event");
    }
    
    // TODO: Run event loop in a seperate thread
}

// Stops the event loop
static VALUE Happening_stop(VALUE self) {
    // TODO: Check return value
    ev_break(happening_loop, EVBREAK_ONE);
}

void Init_happening() {
    happening_loop = EV_DEFAULT;

    Happening = rb_define_module("Happening");
    
    rb_define_module_function(Happening, "next", Happening_next, 0);
    rb_define_module_function(Happening, "in", Happening_in, 1);
    rb_define_module_function(Happening, "at", Happening_at, 1);
    rb_define_module_function(Happening, "when", Happening_when, 2);
    rb_define_module_function(Happening, "start", Happening_start, 0);
    rb_define_module_function(Happening, "run", Happening_run, 0);
    rb_define_module_function(Happening, "stop", Happening_stop, 0);
}

// XXX: How do we de-init and free the event_base?
