export const ThrottleDelay = Symbol("Throttle delay");

/**
 * Wraps a function to throttle its execution based on the specified delay.
 * @param {Function} fn - The function to be executed.
 * @param {number} delay - The delay in milliseconds.
 * @returns {Function} - The throttled function.
 */
export function throttle(fn, delay) {
    let timerId = null;
    let state = {lastArgs: null, timerSetAt: null};

    /**
     * The throttled function that controls the execution of the original function.
     * @param {...any} args - The arguments for the original function.
     */
    function _throttled(...args) {
        if (timerId !== null) {
            // If a timer is already active, store the arguments for later execution.
            state.lastArgs = args;

            // If the timer should have fired but hasn't (possibly due to blocking code),
            //    invoke the function and clear the timer
            if (performance.now() - state.timerSetAt >= delay) {
                clearTimeout(timerId);
                timerId = null;
                state.lastArgs = null;
                _throttled(...args)
            }

            return;
        }

        // Set the timestamp when the timer is activated.
        state.timerSetAt = performance.now();

        timerId = setTimeout(async () => {
            timerId = null;
            // Execute the stored arguments after the delay if any.
            if (state.lastArgs !== null) _throttled(...state.lastArgs);
            state.lastArgs = null;
        }, delay);

        // Invoke the original function immediately if there was no timer
        const ret = fn(...args);

        // Check return value for special symbols
        if (ret instanceof Promise) {
            return ret.then(v => {
                if (v === ThrottleDelay) {
                    // Execute function again later
                    _throttled(...args);
                }

                return v;
            })
        } else if (ret === ThrottleDelay) {
            // Execute function again later
            _throttled(...args);
        }

        return ret;
    }

    return _throttled;
}