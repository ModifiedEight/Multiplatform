#ifdef MCPE_IOS
#import "EAGLView.h"

@implementation EAGLView

@synthesize backingWidth = _backingWidth;
@synthesize backingHeight = _backingHeight;
@synthesize context = _context;

+ (Class)layerClass {
	return [CAEAGLLayer class];
}

- (id)initWithFrame:(CGRect)frame {
	self = [super initWithFrame:frame];
	if (self) {
		if ([self respondsToSelector:@selector(setContentScaleFactor:)]) {
			self.contentScaleFactor = [UIScreen mainScreen].scale;
		}

		CAEAGLLayer* layer = (CAEAGLLayer*)self.layer;
		layer.opaque = YES;
		layer.drawableProperties = @{
			kEAGLDrawablePropertyRetainedBacking : @NO,
			kEAGLDrawablePropertyColorFormat : kEAGLColorFormatRGBA8
		};

		_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		if (!_context || ![EAGLContext setCurrentContext:_context]) {
			NSLog(@"[MCPE] failed to create GLES1 context");
			return nil;
		}
	}
	return self;
}

- (void)makeCurrent {
	[EAGLContext setCurrentContext:_context];
}

- (BOOL)createFramebuffer {
	[EAGLContext setCurrentContext:_context];

	glGenFramebuffersOES(1, &_framebuffer);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, _framebuffer);

	glGenRenderbuffersOES(1, &_colorRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _colorRenderbuffer);
	[_context renderbufferStorage:GL_RENDERBUFFER_OES
	                 fromDrawable:(CAEAGLLayer*)self.layer];
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES,
	                             GL_RENDERBUFFER_OES, _colorRenderbuffer);

	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
	                                GL_RENDERBUFFER_WIDTH_OES, &_backingWidth);
	glGetRenderbufferParameterivOES(GL_RENDERBUFFER_OES,
	                                GL_RENDERBUFFER_HEIGHT_OES, &_backingHeight);

	glGenRenderbuffersOES(1, &_depthRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _depthRenderbuffer);
	// Fancy Graphics renders entity/sky shadows with the stencil buffer
	// (GL_STENCIL_TEST + glStencilFunc). The desktop SDL path requests
	// SDL_GL_STENCIL_SIZE=8 and Android's EGL config asks for EGL_STENCIL_SIZE=8,
	// so a packed 24/8 depth+stencil renderbuffer matches them here. Without a
	// stencil buffer the stencil test passes everywhere and the shadow overlay
	// quad floods the top half of the screen.
	glRenderbufferStorageOES(GL_RENDERBUFFER_OES, GL_DEPTH24_STENCIL8_OES,
	                         _backingWidth, _backingHeight);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES,
	                             GL_RENDERBUFFER_OES, _depthRenderbuffer);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_STENCIL_ATTACHMENT_OES,
	                             GL_RENDERBUFFER_OES, _depthRenderbuffer);

	if (glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES) != GL_FRAMEBUFFER_COMPLETE_OES) {
		NSLog(@"[MCPE] incomplete framebuffer 0x%x",
		      glCheckFramebufferStatusOES(GL_FRAMEBUFFER_OES));
		return NO;
	}
	return YES;
}

- (void)destroyFramebuffer {
	if (_framebuffer) { glDeleteFramebuffersOES(1, &_framebuffer); _framebuffer = 0; }
	if (_colorRenderbuffer) { glDeleteRenderbuffersOES(1, &_colorRenderbuffer); _colorRenderbuffer = 0; }
	if (_depthRenderbuffer) { glDeleteRenderbuffersOES(1, &_depthRenderbuffer); _depthRenderbuffer = 0; }
}

- (void)setFramebuffer {
	[EAGLContext setCurrentContext:_context];
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, _framebuffer);
	glViewport(0, 0, _backingWidth, _backingHeight);
}

- (BOOL)presentFramebuffer {
	[EAGLContext setCurrentContext:_context];
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _colorRenderbuffer);
	return [_context presentRenderbuffer:GL_RENDERBUFFER_OES];
}

- (void)layoutSubviews {
	// Recreate the framebuffer at the new drawable size.
	[self destroyFramebuffer];
	[self createFramebuffer];
}

- (void)dealloc {
	[self destroyFramebuffer];
	if ([EAGLContext currentContext] == _context) {
		[EAGLContext setCurrentContext:nil];
	}
	[_context release];
	[super dealloc];
}

@end
#endif
