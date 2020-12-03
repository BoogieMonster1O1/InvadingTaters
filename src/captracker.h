#ifndef INVADINGTATERS_CAPTRACKER_H
#define INVADINGTATERS_CAPTRACKER_H

#include <GL/glew.h>

class CapabilityTracker {
private:
	GLenum cap;
	bool value;

public:
	CapabilityTracker(GLenum cap, bool def) {
		this->cap = cap;
		this->value = def;
	}

	virtual void enable() {
		set(true);
	}

	virtual void disable() {
		set(false);
	}

	virtual void set(bool val) {
		this->value = val;
		if (val) {
			glEnable(cap);
		} else {
			glDisable(cap);
		}
	}

	virtual bool get() {
		return value;
	}
};

#endif
