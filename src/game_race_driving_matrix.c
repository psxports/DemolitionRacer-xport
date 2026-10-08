#include "draft_signatures.h"
extern void dr_race_quaternion_angles(const sint16 *angles,sint16 *output);
extern void dr_race_quaternion_matrix(const sint16 *quaternion,MATRIX *matrix);
extern void draft_agent3_missing_80049B74(const MATRIX *left,MATRIX *right);
void dr_driving_quaternion(const SVECTOR *angles,SVECTOR *quaternion)
{
    dr_race_quaternion_angles((const sint16*)angles,(sint16*)quaternion);
}
void dr_driving_rotation(const SVECTOR *quaternion,MATRIX *matrix)
{
    dr_race_quaternion_matrix((const sint16*)quaternion,matrix);
}
void dr_driving_matrix(uint32 object,MATRIX *right)
{
    draft_agent3_missing_80049B74((const MATRIX*)psx_addr(object,20u),right);
}
