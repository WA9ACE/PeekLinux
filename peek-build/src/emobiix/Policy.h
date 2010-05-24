#ifndef _POLICY_H_
#define _POLICY_H_

struct Policy_t;
typedef struct Policy_t Policy;

Policy *policy_new(void);
void policy_delete(Policy *policy);


#endif /* _POLICY_H_ */
