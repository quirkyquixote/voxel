
#ifndef VOXEL_FPS_MANAGER_H_
#define VOXEL_FPS_MANAGER_H_

class FpsManager {
public:
	FpsManager(int fps);
	~FpsManager();

	int sync();

private:
	/* Timestamp of the last call to sync that did not happen after the
	 * expected time. It is reset to the current time when sync is called too
	 * late. */
	int t0;

	/* Timestamp of the last call to sync */
	int t1;

	/* Number of times the sync function has been called with time to spare
	 * consecutively. It is reset to zero if the call happens after the
	 * expected time. */
	int frame_count;

	/* Frames per second */
	int fps;
};

#endif
