

#ifndef VOXEL_PROFILE_H_
#define VOXEL_PROFILE_H_

#include <stdint.h>

class Profile {
public:
	Profile(const char *tag);
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


#endif

