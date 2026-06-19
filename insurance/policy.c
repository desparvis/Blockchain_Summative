// insurance/policy.c
#include <stdio.h>
#include <string.h>
#include "policy.h"

Policy policy_registry[MAX_POLICIES];
static int total_policies = 0;

int policy_enroll(const char *member_id, const char *policy_id, const char *plan) {
    if (total_policies >= MAX_POLICIES) return 0;
    
    // Check if policy ID already exists
    for (int i = 0; i < total_policies; i++) {
        if (strcmp(policy_registry[i].policy_id, policy_id) == 0) return 0;
    }

    Policy p;
    strncpy(p.member_id, member_id, ADDR_LEN);
    strncpy(p.policy_id, policy_id, HASH_LEN);
    strncpy(p.coverage_plan, plan, 32);
    
    p.enrollment_date = time(NULL);
    // Explicit rule: Policy expires automatically after exactly 365 days
    p.expiry_date = p.enrollment_date + (365 * 24 * 60 * 60); 
    p.status = POLICY_ACTIVE;

    policy_registry[total_policies] = p;
    total_policies++;
    return 1;
}

PolicyStatus policy_check_and_update_status(const char *policy_id) {
    for (int i = 0; i < total_policies; i++) {
        if (strcmp(policy_registry[i].policy_id, policy_id) == 0) {
            // Check if current system time has sailed past expiration date
            if (time(NULL) > policy_registry[i].expiry_date) {
                policy_registry[i].status = POLICY_EXPIRED;
            }
            return policy_registry[i].status;
        }
    }
    return POLICY_EXPIRED; // Default if non-existent
}

int policy_renew(const char *policy_id) {
    for (int i = 0; i < total_policies; i++) {
        if (strcmp(policy_registry[i].policy_id, policy_id) == 0) {
            // Reset dates as mandated by system specs
            policy_registry[i].enrollment_date = time(NULL);
            policy_registry[i].expiry_date = policy_registry[i].enrollment_date + (365 * 24 * 60 * 60);
            policy_registry[i].status = POLICY_RENEWED;
            return 1;
        }
    }
    return 0;
}

Policy* policy_get(const char *policy_id) {
    for (int i = 0; i < total_policies; i++) {
        if (strcmp(policy_registry[i].policy_id, policy_id) == 0) {
            return &policy_registry[i];
        }
    }
    return NULL;
}
