#ifndef FRAMEHANDLER_H
#define FRAMEHANDLER_H

#include <iostream>
#include <glad\glad.h>

class framebufferHandler{
public:
	unsigned int framebuffer, renderbuffer;
	framebufferHandler() {
		glGenFramebuffers(1, &framebuffer);
	}
	//Renderbuffer obj for depth/stencil passes
	void initiateRenderbuffer(int scrWidth, int scrHeight) {
		glGenRenderbuffers(1, &renderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, scrWidth, scrHeight);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);
	}
	//Check for the framebuffer's completion.
	void framebufferCheck() {
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "Eh, you should have told the framebuffer to complete!!" << std::endl;
		}	
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	//EXTERMINATEEE
	void terminate() {
		glDeleteFramebuffers(1, &framebuffer);
		glDeleteRenderbuffers(1, &renderbuffer);
	}
};

#endif