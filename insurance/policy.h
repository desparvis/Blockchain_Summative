// insurance/policy.h
#ifndef POLICY_H
#define POLICY_H

#include "../core/structures.h"

#define MAX_POLICIES 200

// Enroll a new student/staff member into an insurance plan
int policy_enroll(const char *member_id, const char *policy_id, const char *plan);

// Check if a policy is still active. Auto-expires if time limit is passed
PolicyStatus policy_check_and_update_status(const char *policy_id);

// Renew an expired or expiring policy for an additional 365 days
int policy_renew(const char *policy_id);

// Lookup and fetch a policy record by its ID pointer
Policy* policy_get(const char *policy_id);

#endif // POLICY_H
