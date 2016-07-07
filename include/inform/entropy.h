// Copyright 2016 ELIFE. All rights reserved.
// Use of this source code is governed by a MIT
// license that can be found in the LICENSE file.
#pragma once

#include <inform/dist.h>
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Compute the Shannon self-information of an event given some distribution
 *
 * This function will return `NaN` if the distribution is not valid.
 *
 * @param[in] dist  the probability distribution
 * @param[in] event the event in question
 * @param[in] base  the logarithmic base
 * @return the self-information of the event
 */
double inform_shannon_si(inform_dist const *dist, uint64_t event, double base);

/**
 * Compute the Shannon information of a distribution.
 *
 * This function will return `NaN` if the distribution is not valid,
 * i.e. `!inform_dist_is_valid(dist)`.
 *
 * @param[in] dist the probability distribution
 * @param[in] base the logarithmic base
 * @return the shannon information
 */
double inform_shannon(inform_dist const *dist, double base);

/**
 * Compute the point-wise mutual information of an combination of events
 *
 * @param[in] joint       the joint distribution
 * @param[in] marginal_x  the x-marginal
 * @param[in] marginal_y  the y-marginal
 * @param[in] event_joint the joint event
 * @param[in] event_x     the corresponding event in the x-marginal
 * @param[in] event_y     the corresponding event in the y-marginal
 * @param[in] base        the logarithmic base
 * @return the pointwise mutual information of the events
 */
double inform_shannon_pmi(inform_dist const *joint,
    inform_dist const *marginal_x, inform_dist const *marginal_y,
    uint64_t event_joint, uint64_t event_x, uint64_t enent_y, double base);

/**
 * Compute the Shannon-based mutual information of a distribution and
 * two marginals.
 *
 * This function will return `NaN` if `inform_shannon` returns `NaN`
 * when applied to any of the distribution arguments.
 *
 * @param[in] joint      the joint probability distribution
 * @param[in] marginal_x a marginal distribution
 * @param[in] marginal_y a marginal distribution
 * @param[in] base       the logarithmic base
 * @return the mutual information
 *
 * @see inform_shannon
 */
double inform_mutual_info(inform_dist const *joint,
    inform_dist const *marginal_x,
    inform_dist const *marginal_y,
    double base);

/**
 * Compute the pointwise conditional information of a combination of events
 *
 * @param[in] joint    the joint probability distribution
 * @param[in] marginal the marginal distribution
 * @param[in] event_joint the joint event
 * @param[in] event_marginal the marginal event
 * @param[in] base           the logarithmic base
 * @return the pointwise conditional information of the events
 */
double inform_pointwise_conditional_entropy(inform_dist const *joint,
    inform_dist const *marginal, uint64_t event_joint,
    uint64_t event_marginal, double base);

/**
 * Compute the Shannon-based conditional information of a joint distribution and
 * a marginal.
 *
 * This function will return `NaN` if `inform_shannon` returns `NaN`
 * when applied to any of the distribution arguments.
 *
 * @param[in] joint    the joint probability distribution
 * @param[in] marginal a marginal distribution
 * @param[in] base     the logarithmic base
 * @return the conditional information
 *
 * @see inform_shannon
 */
double inform_conditional_entropy(inform_dist const* joint,
    inform_dist const *marginal,
    double base);

/**
 * Compute the pointwise conditional mutual information of a combination of events
 *
 * @param[in] joint             the joint probability distribution
 * @param[in] marginal_xz       the xz-marginal
 * @param[in] marginal_yz       the yz-marginal
 * @param[in] marginal_z        the z-marginal
 * @param[in] event_joint       the joint event
 * @param[in] event_marginal_xz the xz event
 * @param[in] event_marginal_yz the yz event
 * @param[in] event_marginal_z  the z event
 * @param[in] base              the logarithmic base
 * @return the pointwise conditional mutual information
 */
double inform_pointwise_conditional_mutual_info(inform_dist const *joint,
    inform_dist const *marginal_xz, inform_dist const *marginal_yz,
    inform_dist const *marginal_z, uint64_t event_joint,
    uint64_t event_marginal_xz, uint64_t event_marginal_yz,
    uint64_t event_marginal_z, double base);

/**
 * Compute the conditional mutual entropy of a joint distribution, and
 * the xz-, yz, and z-marginals.
 *
 * @param[in] joint       then joint probability distribution
 * @param[in] marginal_xz the xz-marginal
 * @param[in] marginal_yz the yz-marginal
 * @param[in] marginal_z  the z-marginal
 * @param[in] base        the logarithmic base
 * @return the conditional mutual information;
 */
double inform_conditional_mutual_info(inform_dist const *joint,
    inform_dist const *marginal_xz, inform_dist const *marginal_yz,
    inform_dist const *marginal_z, double base);

#ifdef __cplusplus
}
#endif
