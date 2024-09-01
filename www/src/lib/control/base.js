/**
 * @class
 * @template T
 */
export class Control {
    /**
     * @param {HTMLElement} element
     */
    constructor(element) {
        this.element = element;
    }

    /**
     *
     * @param {string} id
     * @param {...*} params
     * @return {T}
     */
    static byId(id, ...params) {
        const e = document.getElementById(id);
        if (!e) {
            throw new Error(`Unable to fined element ${id}`);
        }

        return /** @type {T} */ new this(e, ...params);
    }

    setVisibility(show) {
        this.element.style.display = show ? null : "none";
    }

    setEnabled(enabled) {
        if (enabled) {
            this.element.removeAttribute("disabled");
        } else {
            this.element.setAttribute("disabled", "");
        }

    }

    setTooltip(text) {
        this.element.setAttribute("title", text);
    }

    setInteractions(enable) {
        this.element.style.pointerEvents = enable ? null : "none";
    }

    addClass(className) {
        this.element.classList.add(className);
    }

    removeClass(className) {
        this.element.classList.remove(className);
    }

    setAttribute(attribName, value) {
        this.element.setAttribute(attribName, value.toString());
    }

    getAttribute(attribName) {
        return this.element.getAttribute(attribName);
    }
}

export class InputControlBase extends Control {
    _onChangeFn = null;

    /**
     * @param {HTMLInputElement} element
     */
    constructor(element) {
        super(element);
    }

    /**
     * @abstract
     */
    getValue() {
    }

    setValue(value) {
        this.element.value = value;
    }

    setOnChange(fn) {
        this._onChangeFn = fn;
    }

    /**
     * @param {*} value
     * @param {*} prevValue
     * @protected
     */
    _emitChanged(value, prevValue) {
        if (this._onChangeFn) {
            this._onChangeFn(value, prevValue);
        }
    }
}