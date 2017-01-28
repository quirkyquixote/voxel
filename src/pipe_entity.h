/* Copyright 2017 Luis Sanz <luis.sanz@gmail.com> */

#ifndef SRC_PIPE_ENTITY_H_
#define SRC_PIPE_ENTITY_H_

#include "block_entity.h"

class PipeEntity : public BlockEntity {
 public:
	explicit PipeEntity(Context *ctx);
	~PipeEntity();

	serializer::Tag *save();
	void load(serializer::Tag *val);
	bool use(PlayerEntity *player);

	const char *get_name() { return "pipe"; }
};

#endif  // SRC_PIPE_ENTITY_H_
