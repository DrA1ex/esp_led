export class EventEmitter {
    subscribers = new Map();

    /**
     * @param {string} type
     * @param {*|null} data
     */
    emitEvent(type, data = null) {
        for (let subscriptions of this.subscribers.values()) {
            const handler = subscriptions[type];
            if (handler) {
                try {
                    handler(this, data);
                } catch (e) {
                    console.error("Error while event handling", e);
                }
            }
        }
    }

    /**
     * @param {object} subscriber
     * @param {string} type
     * @param {function} handler
     */
    subscribe(subscriber, type, handler) {
        if (!this.subscribers.has(subscriber)) {
            this.subscribers.set(subscriber, {});
        }

        const subscription = this.subscribers.get(subscriber);
        subscription[type] = handler;
    }

    /**
     * @param {object} subscriber
     * @param {string} type
     */
    unsubscribe(subscriber, type) {
        const subscription = this.subscribers.has(subscriber) ? this.subscribers.get(subscriber) : null;
        if (subscription && subscription.hasOwnProperty(type)) {
            delete subscription[type];
        }
    }
}