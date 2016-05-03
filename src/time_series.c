// Copyright 2016 ELIFE. All rights reserved.
// Use of this source code is governed by a MIT
// license that can be found in the LICENSE file.
#include <inform/state_encoding.h>
#include <inform/time_series.h>

int inform_active_info_dist(uint64_t const* series, size_t n,
                            uint64_t b, uint64_t k, inform_dist *states,
                            inform_dist *histories,
                            inform_dist *futures)
{
    // store a pointer to the end of the time series
    uint64_t const *last   = series + n;
    // store a pointer to the future state
    uint64_t const *future = series + k;
    // while the future state is in the time series
    while (future != last)
    {
        // encode the k-length history as an integer
        uint64_t const history = inform_encode(series, k, b);
        // if the encoding failed, return an error code
        if (history >= INFORM_ENCODING_ERROR(0))
        {
            return 1;
        }
        // construct an encoding of the state (history + future) as an integer
        uint64_t const state   = history + (*future * powl(b, k));

        // log observations of the state, history and future
        inform_dist_tick(states, state);
        inform_dist_tick(histories, history);
        inform_dist_tick(futures, *future);

        // move to the next time step
        ++series;
        ++future;
    }
    return 0;
}

entropy inform_active_info(uint64_t const *series, size_t n, uint64_t b, uint64_t k)
{
    return inform_active_info_ensemble(series, 1, n, b, k);
}

entropy inform_active_info_ensemble(uint64_t const *series, size_t n, size_t m, uint64_t b, uint64_t k)
{
    // ensure that the time series is not NULL
    if (series == NULL)
    {
        return nan("1");
    }
    // ensure that the dimensions of the time series make sense
    else if (m <= 1 || n < 1)
    {
        return nan("2");
    }
    // ensure that the number of time steps greater than the history length
    else if (m <= k)
    {
        return nan("3");
    }

    // allocate a distribution for the observed states, histories and futures
    inform_dist *states    = inform_dist_alloc(powl(b,k+1));
    inform_dist *histories = inform_dist_alloc(powl(b,k));
    inform_dist *futures   = inform_dist_alloc(b);

    // for each initial condition
    for (uint64_t i = 0; i < n; ++i, series += m)
    {
        // accumulate observations
        if (inform_active_info_dist(series, m, b, k, states, histories, futures) != 0)
        {
            inform_dist_free(futures);
            inform_dist_free(histories);
            inform_dist_free(states);
            return nan("4");
        }
    }
    // compute the mututal information between the states, histories and futures,
    // i.e. the active information
    entropy ai = inform_mutual_info(states, histories, futures, b);

    // free up the distributions (otherwise there would be memory leaks)
    inform_dist_free(futures);
    inform_dist_free(histories);
    inform_dist_free(states);

    // return the active information
    return ai;
}

int inform_transfer_entropy_dist(uint64_t const *series_y,
                                 uint64_t const *series_x, size_t n,
                                 uint64_t b, uint64_t k,
                                 inform_dist *states,
                                 inform_dist *histories,
                                 inform_dist *sources,
                                 inform_dist *predicates)
{
    // store a pointer to the end of the target series
    uint64_t const *last    = series_x + n;
    // store a pointer to the future state of the target series
    uint64_t const *future  = series_x + k;
    // store a pointer to the current state of the source series
    uint64_t const *y_state = series_y + k - 1;
    // while the future state is in the target series
    while (future != last)
    {
        // encode the k-length history as an integer
        uint64_t const history = inform_encode(series_x, k, b);
        // if the encoding failed or either of the future state and y-state are
        // not valid given the base, return an error code
        if (history >= INFORM_ENCODING_ERROR(0) || *future >= b || *y_state >= b)
        {
            return 1;
        }
        // construct an encoding of the state (history + future + y-state)
        uint64_t const state     = history + (*future * powl(b, k)) + (*y_state * powl(b, k+1));
        // construct an encoding of the sources (history + ystate)
        uint64_t const source    = history + (*y_state * powl(b, k));
        // construct an encoding of the predicates (history + future)
        uint64_t const predicate = history + (*future * powl(b, k));

        // log observations of the state, history, source and predicate
        inform_dist_tick(states, state);
        inform_dist_tick(histories, history);
        inform_dist_tick(sources, source);
        inform_dist_tick(predicates, predicate);

        // move to the next time step
        ++series_x;
        ++y_state;
        ++future;
    }
    return 0;
}

entropy inform_transfer_entropy(uint64_t const *node_y, uint64_t const *node_x, size_t n, uint64_t b, uint64_t k)
{
    return inform_transfer_entropy_ensemble(node_y, node_x, 1, n, b, k);
}

entropy inform_transfer_entropy_ensemble(uint64_t const *node_y, uint64_t const *node_x, size_t n, size_t m, uint64_t b, uint64_t k)
{
    // ensure that neither of the time series are NULL
    if (node_x == NULL || node_y == NULL)
    {
        return nan("1");
    }
    // ensure that the dimensions of the time series make sense
    else if (m <= 1 || n < 1)
    {
        return nan("2");
    }
    // ensure that the number of time steps greater than the history length
    else if (m <= k)
    {
        return nan("3");
    }

    // allocate a distribution for the observed states, histories,
    // sources and predicates
    inform_dist *states     = inform_dist_alloc(powl(b,k+2));
    inform_dist *histories  = inform_dist_alloc(powl(b,k));
    inform_dist *sources    = inform_dist_alloc(powl(b,k+1));
    inform_dist *predicates = inform_dist_alloc(powl(b,k+1));

    // for each initial condition
    for (uint64_t i = 0; i < n; ++i, node_x += m, node_y += m)
    {
        // accumulate observations and return NaN if there is an encoding error
        if (inform_transfer_entropy_dist(node_y, node_x, m, b, k, states, histories, sources, predicates) != 0)
        {
            inform_dist_free(predicates);
            inform_dist_free(sources);
            inform_dist_free(histories);
            inform_dist_free(states);
            return nan("4");
        }
    }
    // compute the transfer entropy from the distributions
    entropy te = inform_shannon(sources, b) + inform_shannon(predicates, b) -
        inform_shannon(states, b) - inform_shannon(histories, b);

    // free up the distributions (otherwise there would be memory leaks)
    inform_dist_free(predicates);
    inform_dist_free(sources);
    inform_dist_free(histories);
    inform_dist_free(states);

    // return the transfer entropy
    return te;
}
