/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_PROFILE_H_
#define SRC_PROFILE_H_

#include <stdint.h>

class Profile {
 public:
	explicit Profile(const char *tag);
	virtual ~Profile();
	virtual void begin();
	virtual void end();
	virtual void update();

 protected:
	char *tag;
	long long acc;
	long long samples;
};

class CpuProfile : public Profile {
 public:
	CpuProfile();
	~CpuProfile();
	void begin();
	void end();
	void update();

 private:
	long long time;
};

class GpuProfile : public Profile {
 public:
	GpuProfile();
	~GpuProfile();
	void begin();
	void end();
	void update();

 private:
	long long time;
};


#endif  // SRC_PROFILE_H_

