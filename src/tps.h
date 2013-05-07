/**
 * Thin-plate spline (tps)
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tps_t *tps_t;

// Constructors/destructors
// ------------------------
// If xs and ys are NULL, make_tps() will just allocated space.
// Otherwise, it uses tps_fit() to initialize the coefficients.
// If not NULL, xs and ys must be row-major ordered matrices
// of dimension (nknots,dim+1).  The extra dimension is for the
// affine space and the values in that dimension should be
// initialized to 1.
tps_t make_tps(float stiffness, int nknots, int dim, float *ys, float *xs);
void  free_tps(tps_t ctx);

// The good stuff
// --------------
// tps_eval()   solve for coefficients that minimize the error between xs and ys
//              subject to the model.
//              - xs must be not NULL on the first call to eval, but
//                may be NULL for subsiquent calls, in which case the
//                last set of xs is used.  If npoints changes, a new
//                xs must be provided.
// tps_fit()    Applies the model to xs and stores the result in ys.
// tps_update() Refit by adjusting the knot positions in the destination space.
// 
// xs and ys are both row-major two-dimensional matrices of dimension
// (npoints,dim+1).  For tps_fit(), npoints must be the same as nknots.
// xs are in the source space, and ys are in the destination space.
tps_t tps_fit   (tps_t ctx, float *ys,float *xs,float stiffness);
tps_t tps_update(tps_t ctx, float *ys,float stiffness);
tps_t tps_eval  (tps_t ctx,int npoints,float *ys,float *xs);

float tps_energy(tps_t ctx); ///< returns bending energy of the fit in units of stiffness, or 0 if no fit has been performed.
// Error reporting callbacks
// -------------------------
// The reporters will be called for logging purposes.
// file, line and function will be the name of the file, the line, and 
// the function name where the reporter is called.  The format string
// is a printf-style format specifier.
// 
// It's not necessary to set a reporter; printf is the default.
// You can use the following functions to set a global default reporter,
// or set reporters specific to a give tps context.
// 
// Setting a field to NULL in the tps_reporter_t struct is ok; that field will 
// be ignored.
typedef int (*tps_reporter_t)(const char *file,int line,const char *function,const char *format,...);
typedef struct _tps_reporters_t
{ tps_reporter_t error,
                 warning,
                 debug;
} tps_reporters_t;
void tps_set_reporters        (tps_t ctx, tps_reporters_t rs);
void tps_set_default_reporters(tps_reporters_t rs);

#if 0 //To implement?
// Parameters
// ----------
// 
// Changing parameters does not recompute the fit.
// 
// - Kernel
//   The default is the r^2 log r kernel for thin plate splines, but if you want 
//   to change it, who am I to stop you?
// - stiffness

typedef float (*tps_kernel_t)(int ndim, float *x1, float *x2);
void  tps_set_kernel(tps_t ctx,tps_kernel_t kernel);
void  tps_set_stiffness(tps_t ctx, float stiffness);
#endif
float tps_stiffness(tps_t ctx);
int   tps_npoints(tps_t ctx); ///< returns number of points in the last query
int   tps_ndims(tps_t ctx);   ///< returns number of dimensions used for the last query


#ifdef __cplusplus
} //extern "C"
#endif