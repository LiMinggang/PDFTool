/* Copyright (C) 1996, 1997 Aladdin Enterprises.  All rights reserved.
   Unauthorized use, copying, and/or distribution prohibited.
 */

/* pltop.h */
/* Interface to main API for interpreters */

#ifndef pltop_INCLUDED
#  define pltop_INCLUDED

#include "gsgc.h"
#include "scommon.h"

#ifndef gx_device_DEFINED
#  define gx_device_DEFINED
typedef struct gx_device_s gx_device;
#endif

/*
 * Generic interpreter data types which may be subclassed by specific interpereters
 */
typedef struct pl_interp_implementation_s pl_interp_implementation_t;	/* fwd decl */
typedef struct pl_interp_s {
	const struct pl_interp_implementation_s  *implementation;  /* implementation of actual interp */
} pl_interp_t;

typedef struct pl_interp_instance_s {
	pl_interp_t     *interp;            /* interpreter instance refers to */
	vm_spaces       spaces;             /* spaces for GC */
        char *          pcl_personality;
} pl_interp_instance_t;

/* Param data types */
typedef int (*pl_page_action_t)(P2(pl_interp_instance_t *, void *));

/*
 * Implementation characteristics descriptor
 */
typedef struct pl_interp_characteristics_s {
  const char*                 language;          /* generic language should correspond with
						    HP documented PJL name */
  const char*                 auto_sense_string;  /* string used to detect language */
  const char*                 manufacturer;      /* manuf str */
  const char*                 version;           /* version str */
  const char*                 build_date;        /* build date str */
  int                         min_input_size;    /* min sizeof input buffer */
} pl_interp_characteristics_t;

/*
 * The pl_interp_t and pl_interp_instance are intended to provide a generic
 * front end for language interpreters, in tandem with a 
 * pl_interp_implementation_t. pl_interp_t and pl_interp_impmementation_t
 * together are used to describe a particular implementation. An implementation
 * can then generate one or more instances, which are more-or-less
 * independent sessions.
 *
 * The pattern for a client using these objects is:
 *  match desired characteristics vs. pl_characteristics(&an_implementation);
 *  pl_allocate_interp(&interp, &an_implementation, ...);
 *  for (1 or more sessions)
 *    pl_allocate_interp_instance(&instance, interp, ...);
 *    pl_set_client_instance(instance, ...); // lang-specific client (e.g. PJL)
 *    pl_set_pre_page_action(instance, ...); // opt rtn called B4 each pageout
 *    pl_set_post_page_action(instance,...); // opt rtn called after pageout
 *    for (each device that needs output)
 *      pl_set_device(instance, device);  //device is already open
 *      for (each print job)
 *        pl_init_job(instance)
 *        while (!end of job stream && no error)
 *          pl_process(instance, cursor);
 *        if (error || (end of input stream && pl_process didn't end normally yet))
 *          while (!pl_flush_to_eoj(instance, cursor))
 *            ; // positions cursor at eof or 1 past EOD marker
 *        if (end of input stream &&n pl_process didnt' end normally yet)
 *          pl_process_eof(instance);  // will reset instance's parser state
 *        if (errors)
 *          pl_report_errors(instance, ...);
 *        pl_dnit_job(instance);
 *      pl_remove_device(instance);  //device still open
 *    pl_deallocate_interp_instance(instance);
 *  pl_deallocte_interp(interp);
 *
 * Notice that this API allows you to have multiple instances, of multiple
 * implementations, open at once, but some implementations may impose restrictions
 * on the number of instances that may be open at one time (e.g. one).
 */

/*
 * Define interp procedures: See comments in pltop.c for descriptions/ret vals
 */
const pl_interp_characteristics_t * pl_characteristics(P1(const pl_interp_implementation_t *));
typedef const pl_interp_characteristics_t * (*pl_interp_proc_characteristics_t)(P1(const pl_interp_implementation_t *));

int pl_allocate_interp(P3(pl_interp_t **, const pl_interp_implementation_t *, gs_memory_t *));
typedef int (*pl_interp_proc_allocate_interp_t)(P3(pl_interp_t **, const pl_interp_implementation_t *, gs_memory_t *));

int pl_allocate_interp_instance(P3(pl_interp_instance_t **, pl_interp_t *, gs_memory_t *));
typedef int (*pl_interp_proc_allocate_interp_instance_t)(P3(pl_interp_instance_t **, pl_interp_t *, gs_memory_t *));

int pl_set_client_instance(P2(pl_interp_instance_t *, pl_interp_instance_t *));
typedef int (*pl_interp_proc_set_client_instance_t)(P2(pl_interp_instance_t *, pl_interp_instance_t *));

int pl_set_pre_page_action(P3(pl_interp_instance_t *, pl_page_action_t, void *));
typedef int (*pl_interp_proc_set_pre_page_action_t)(P3(pl_interp_instance_t *, pl_page_action_t, void *));

int pl_set_post_page_action(P3(pl_interp_instance_t *, pl_page_action_t, void *));
typedef int (*pl_interp_proc_set_post_page_action_t)(P3(pl_interp_instance_t *, pl_page_action_t, void *));

int pl_set_device(P2(pl_interp_instance_t *, gx_device *));
typedef int (*pl_interp_proc_set_device_t)(P2(pl_interp_instance_t *, gx_device *));

int pl_init_job(P1(pl_interp_instance_t *));
typedef int (*pl_interp_proc_init_job_t)(P1(pl_interp_instance_t *));

int pl_process(P2(pl_interp_instance_t *, stream_cursor_read *));
typedef int (*pl_interp_proc_process_t)(P2(pl_interp_instance_t *, stream_cursor_read *));

int pl_flush_to_eoj(P2(pl_interp_instance_t *, stream_cursor_read *));
typedef int (*pl_interp_proc_flush_to_eoj_t)(P2(pl_interp_instance_t *,  stream_cursor_read *));

int pl_process_eof(P1(pl_interp_instance_t *));
typedef int (*pl_interp_proc_process_eof_t)(P1(pl_interp_instance_t *));

int pl_report_errors(P5(pl_interp_instance_t *, int, long, bool, FILE *));
typedef int (*pl_interp_proc_report_errors_t)(P5(pl_interp_instance_t *, int, long, bool, FILE *));

int pl_dnit_job(P1(pl_interp_instance_t *));
typedef int (*pl_interp_proc_dnit_job_t)(P1(pl_interp_instance_t *));

int pl_remove_device(P1(pl_interp_instance_t *));
typedef int (*pl_interp_proc_remove_device_t)(P1(pl_interp_instance_t *));

int pl_deallocate_interp_instance(P1(pl_interp_instance_t *));
typedef int (*pl_interp_proc_deallocate_interp_instance_t)(P1(pl_interp_instance_t *));

int pl_deallocate_interp(P1(pl_interp_t *));
typedef int (*pl_interp_proc_deallocate_interp_t)(P1(pl_interp_t *));

/*
 * Define a generic interpreter implementation
 */
struct pl_interp_implementation_s {
	/* Procedure vector */
  pl_interp_proc_characteristics_t            proc_characteristics;
  pl_interp_proc_allocate_interp_t            proc_allocate_interp;
  pl_interp_proc_allocate_interp_instance_t   proc_allocate_interp_instance;
  pl_interp_proc_set_client_instance_t        proc_set_client_instance;
  pl_interp_proc_set_pre_page_action_t        proc_set_pre_page_action;
  pl_interp_proc_set_post_page_action_t       proc_set_post_page_action;
  pl_interp_proc_set_device_t                 proc_set_device;
  pl_interp_proc_init_job_t                   proc_init_job;
  pl_interp_proc_process_t                    proc_process;
  pl_interp_proc_flush_to_eoj_t               proc_flush_to_eoj;
  pl_interp_proc_process_eof_t                proc_process_eof;
  pl_interp_proc_report_errors_t              proc_report_errors;
  pl_interp_proc_dnit_job_t                   proc_dnit_job;
  pl_interp_proc_remove_device_t              proc_remove_device;
  pl_interp_proc_deallocate_interp_instance_t proc_deallocate_interp_instance;
  pl_interp_proc_deallocate_interp_t          proc_deallocate_interp;
};

#endif				/* pltop_INCLUDED */
