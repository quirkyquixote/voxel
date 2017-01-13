
#include "types.h"

const char *obj_names[] = {
	"block", "slab", "stairs", "pane", "fluid (debug)",
	"mat05", "mat06", "mat07",
	"obj08", "obj09", "obj0A", "obj0B", "obj0C", "obj0D", "obj0E", "obj0F",
	"obj10", "obj11", "obj12", "obj13", "obj14", "obj15", "obj16", "obj17",
	"obj18", "obj19", "obj1A", "obj1B", "obj1C", "obj1D", "obj1E", "obj1F",
	"obj20", "obj21", "obj22", "obj23", "obj24", "obj25", "obj26", "obj27",
	"obj28", "obj29", "obj2A", "obj2B", "obj2C", "obj2D", "obj2E", "obj2F",
	"obj30", "obj31", "obj32", "obj33", "obj34", "obj35", "obj36", "obj37",
	"obj38", "obj39", "obj3A", "obj3B", "obj3C", "obj3D", "obj3E", "obj3F",
	"obj40", "obj41", "obj42", "obj43", "obj44", "obj45", "obj46", "obj47",
	"obj48", "obj49", "obj4A", "obj4B", "obj4C", "obj4D", "obj4E", "obj4F",
	"obj50", "obj51", "obj52", "obj53", "obj54", "obj55", "obj56", "obj57",
	"obj58", "obj59", "obj5A", "obj5B", "obj5C", "obj5D", "obj5E", "obj5F",
	"obj60", "obj61", "obj62", "obj63", "obj64", "obj65", "obj66", "obj67",
	"obj68", "obj69", "obj6A", "obj6B", "obj6C", "obj6D", "obj6E", "obj6F",
	"obj70", "obj71", "obj72", "obj73", "obj74", "obj75", "obj76", "obj77",
	"obj78", "obj79", "obj7A", "obj7B", "obj7C", "obj7D", "obj7E", "obj7F",
	"obj80", "obj81", "obj82", "obj83", "obj84", "obj85", "obj86", "obj87",
	"obj88", "obj89", "obj8A", "obj8B", "obj8C", "obj8D", "obj8E", "obj8F",
	"obj90", "obj91", "obj92", "obj93", "obj94", "obj95", "obj96", "obj97",
	"obj98", "obj99", "obj9A", "obj9B", "obj9C", "obj9D", "obj9E", "obj9F",
	"objA0", "objA1", "objA2", "objA3", "objA4", "objA5", "objA6", "objA7",
	"objA8", "objA9", "objAA", "objAB", "objAC", "objAD", "objAE", "objAF",
	"objB0", "objB1", "objB2", "objB3", "objB4", "objB5", "objB6", "objB7",
	"objB8", "objB9", "objBA", "objBB", "objBC", "objBD", "objBE", "objBF",
	"objC0", "objC1", "objC2", "objC3", "objC4", "objC5", "objC6", "objC7",
	"objC8", "objC9", "objCA", "objCB", "objCC", "objCD", "objCE", "objCF",
	"objD0", "objD1", "objD2", "objD3", "objD4", "objD5", "objD6", "objD7",
	"objD8", "objD9", "objDA", "objDB", "objDC", "objDD", "objDE", "objDF",
	"objE0", "objE1", "objE2", "objE3", "objE4", "objE5", "objE6", "objE7",
	"objE8", "objE9", "objEA", "objEB", "objEC", "objED", "objEE", "objEF",
	"objF0", "objF1", "objF2", "objF3", "objF4", "objF5", "objF6", "objF7",
	"objF8", "objF9", "objFA", "objFB", "objFC", "objFD", "objFE", "objFF"
};

const char *mat_names[] = {
	"limestone", "dirt", "wood", "tile", "grass", "stonebrick",
	"workbench", "crate", "pipe", "cobble",
	"mat0A", "mat0B", "mat0C", "mat0D", "mat0E", "mat0F",
	"mat10", "mat11", "mat12", "mat13", "mat14", "mat15", "mat16", "mat17",
	"mat18", "mat19", "mat1A", "mat1B", "mat1C", "mat1D", "mat1E", "mat1F",
	"mat20", "mat21", "mat22", "mat23", "mat24", "mat25", "mat26", "mat27",
	"mat28", "mat29", "mat2A", "mat2B", "mat2C", "mat2D", "mat2E", "mat2F",
	"mat30", "mat31", "mat32", "mat33", "mat34", "mat35", "mat36", "mat37",
	"mat38", "mat39", "mat3A", "mat3B", "mat3C", "mat3D", "mat3E", "mat3F",
	"mat40", "mat41", "mat42", "mat43", "mat44", "mat45", "mat46", "mat47",
	"mat48", "mat49", "mat4A", "mat4B", "mat4C", "mat4D", "mat4E", "mat4F",
	"mat50", "mat51", "mat52", "mat53", "mat54", "mat55", "mat56", "mat57",
	"mat58", "mat59", "mat5A", "mat5B", "mat5C", "mat5D", "mat5E", "mat5F",
	"mat60", "mat61", "mat62", "mat63", "mat64", "mat65", "mat66", "mat67",
	"mat68", "mat69", "mat6A", "mat6B", "mat6C", "mat6D", "mat6E", "mat6F",
	"mat70", "mat71", "mat72", "mat73", "mat74", "mat75", "mat76", "mat77",
	"mat78", "mat79", "mat7A", "mat7B", "mat7C", "mat7D", "mat7E", "mat7F",
	"mat80", "mat81", "mat82", "mat83", "mat84", "mat85", "mat86", "mat87",
	"mat88", "mat89", "mat8A", "mat8B", "mat8C", "mat8D", "mat8E", "mat8F",
	"mat90", "mat91", "mat92", "mat93", "mat94", "mat95", "mat96", "mat97",
	"mat98", "mat99", "mat9A", "mat9B", "mat9C", "mat9D", "mat9E", "mat9F",
	"matA0", "matA1", "matA2", "matA3", "matA4", "matA5", "matA6", "matA7",
	"matA8", "matA9", "matAA", "matAB", "matAC", "matAD", "matAE", "matAF",
	"matB0", "matB1", "matB2", "matB3", "matB4", "matB5", "matB6", "matB7",
	"matB8", "matB9", "matBA", "matBB", "matBC", "matBD", "matBE", "matBF",
	"matC0", "matC1", "matC2", "matC3", "matC4", "matC5", "matC6", "matC7",
	"matC8", "matC9", "matCA", "matCB", "matCC", "matCD", "matCE", "matCF",
	"matD0", "matD1", "matD2", "matD3", "matD4", "matD5", "matD6", "matD7",
	"matD8", "matD9", "matDA", "matDB", "matDC", "matDD", "matDE", "matDF",
	"matE0", "matE1", "matE2", "matE3", "matE4", "matE5", "matE6", "matE7",
	"matE8", "matE9", "matEA", "matEB", "matEC", "matED", "matEE", "matEF",
	"matF0", "matF1", "matF2", "matF3", "matF4", "matF5", "matF6", "matF7",
	"matF8", "matF9", "matFA", "matFB", "matFC", "matFD", "matFE", "foo"
};

const char *shape_names[] = {
	"none",
	"block_dn", "block_up", "block_lf", "block_rt", "block_bk", "block_ft",
	"slab_dn", "slab_up", "slab_lf", "slab_rt", "slab_bk", "slab_ft",
	"pane_x", "pane_y", "pane_z",
	"stairs_df", "stairs_dl", "stairs_db", "stairs_dr",
	"stairs_uf", "stairs_ul", "stairs_ub", "stairs_ur",
	"sha24", "sha25", "sha26", "sha27",
	"sha28", "sha29", "sha2A", "sha2B", "sha2C", "sha2D", "sha2E", "sha2F",
	"sha30", "sha31", "sha32", "sha33", "sha34", "sha35", "sha36", "sha37",
	"sha38", "sha39", "sha3A", "sha3B", "sha3C", "sha3D", "sha3E", "sha3F",
	"sha40", "sha41", "sha42", "sha43", "sha44", "sha45", "sha46", "sha47",
	"sha48", "sha49", "sha4A", "sha4B", "sha4C", "sha4D", "sha4E", "sha4F",
	"sha50", "sha51", "sha52", "sha53", "sha54", "sha55", "sha56", "sha57",
	"sha58", "sha59", "sha5A", "sha5B", "sha5C", "sha5D", "sha5E", "sha5F",
	"sha60", "sha61", "sha62", "sha63", "sha64", "sha65", "sha66", "sha67",
	"sha68", "sha69", "sha6A", "sha6B", "sha6C", "sha6D", "sha6E", "sha6F",
	"sha70", "sha71", "sha72", "sha73", "sha74", "sha75", "sha76", "sha77",
	"sha78", "sha79", "sha7A", "sha7B", "sha7C", "sha7D", "sha7E", "sha7F",
	"sha80", "sha81", "sha82", "sha83", "sha84", "sha85", "sha86", "sha87",
	"sha88", "sha89", "sha8A", "sha8B", "sha8C", "sha8D", "sha8E", "sha8F",
	"sha90", "sha91", "sha92", "sha93", "sha94", "sha95", "sha96", "sha97",
	"sha98", "sha99", "sha9A", "sha9B", "sha9C", "sha9D", "sha9E", "sha9F",
	"shaA0", "shaA1", "shaA2", "shaA3", "shaA4", "shaA5", "shaA6", "shaA7",
	"shaA8", "shaA9", "shaAA", "shaAB", "shaAC", "shaAD", "shaAE", "shaAF",
	"shaB0", "shaB1", "shaB2", "shaB3", "shaB4", "shaB5", "shaB6", "shaB7",
	"shaB8", "shaB9", "shaBA", "shaBB", "shaBC", "shaBD", "shaBE", "shaBF",
	"shaC0", "shaC1", "shaC2", "shaC3", "shaC4", "shaC5", "shaC6", "shaC7",
	"shaC8", "shaC9", "shaCA", "shaCB", "shaCC", "shaCD", "shaCE", "shaCF",
	"shaD0", "shaD1", "shaD2", "shaD3", "shaD4", "shaD5", "shaD6", "shaD7",
	"shaD8", "shaD9", "shaDA", "shaDB", "shaDC", "shaDD", "shaDE", "shaDF",
	"shaE0", "shaE1", "shaE2", "shaE3", "shaE4", "shaE5", "shaE6", "shaE7",
	"shaE8", "shaE9", "shaEA", "shaEB", "shaEC", "shaED", "shaEE", "shaEF",
	"shaF0", "shaF1", "shaF2", "shaF3", "shaF4", "shaF5", "shaF6", "shaF7",
	"shaF8", "shaF9", "shaFA", "shaFB", "shaFC", "shaFD", "shaFE", "shaFF"
};

const char *face_names[] = {
	"front",
	"left",
	"back",
	"right",
	"up",
	"down",
};

