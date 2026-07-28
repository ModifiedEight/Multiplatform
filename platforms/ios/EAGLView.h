#ifdef MCPE_IOS
#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>

// A CAEAGLLayer-backed view that owns the GLES1 framebuffer the game renders
// into. The view controller drives presentation each CADisplayLink tick.
@interface EAGLView : UIView {
@private
	EAGLContext* _context;
	GLuint _framebuffer;
	GLuint _colorRenderbuffer;
	GLuint _depthRenderbuffer;
	GLint _backingWidth;
	GLint _backingHeight;
}

@property (nonatomic, readonly) GLint backingWidth;
@property (nonatomic, readonly) GLint backingHeight;
@property (nonatomic, readonly) EAGLContext* context;

- (BOOL)createFramebuffer;
- (void)destroyFramebuffer;
- (void)setFramebuffer;       // bind + viewport
- (BOOL)presentFramebuffer;   // swap
- (void)makeCurrent;

@end
#endif
