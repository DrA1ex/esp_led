import {InputControlBase} from "./base.js";

export class TriggerControl extends InputControlBase {
    constructor(element) {
        super(element)

        this.addClass("button")
        this.setValue(false);

        element.onclick = this.#onClick.bind(this);
    }

    setValue(flag) {
        this.setAttribute("data-value", flag.toString());
    }

    getValue() {
        return this.getAttribute("data-value") === "true";
    }

    #onClick() {
        const oldValue = this.getValue();
        const nextValue = !oldValue;

        this.setValue(nextValue);
        this._emitChanged(nextValue, oldValue);
    }
}