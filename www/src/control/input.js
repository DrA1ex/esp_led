import {InputControlBase} from "./base.js";

/**
 * @enum{number}
 */
export const InputType = {
    text: "text",
    int: "int",
    float: "float",
    time: "time"
}

export class InputControl extends InputControlBase {
    #value;

    /**
     * @param {HTMLInputElement} element
     * @param {InputType} [type=InputType.text]
     */
    constructor(element, type = InputType.text) {
        super(element);

        this.addClass("input");

        this.type = type;
        this.element.type = type === InputType.time ? "time" : "text";
        this.element.onchange = this._onChange.bind(this);

        switch (type) {
            case InputType.time:
                this.element.onfocus = () => this.element.showPicker();
                break;
        }
    }

    getValue() {
        if (!this.isValid()) {
            return null;
        }

        switch (this.type) {
            case InputType.int:
                return Number.parseInt(this.element.value);

            case InputType.float:
                return Number.parseFloat(this.element.value);

            case InputType.time:
                return this.element.valueAsNumber / 1000;

            default:
                return this.element.value;
        }
    }

    setValue(value) {
        switch (this.type) {
            case InputType.time:
                this.element.valueAsNumber = value * 1000;
                break;

            default:
                super.setValue(value);
        }

        this.#value = this.getValue();
    }

    isValid() {
        switch (this.type) {
            case InputType.int:
                return Number.isFinite(Number.parseInt(this.element.value));

            case InputType.float:
                return Number.isFinite(Number.parseFloat(this.element.value));

            case InputType.time:
                return Number.isFinite(this.element.valueAsNumber);

            default:
                return this.element.validity.valid;
        }
    }

    _onChange() {
        const valid = this.isValid();
        this.setAttribute("invalid", `${!valid}`);

        if (valid) {
            const oldValue = this.#value;
            this.#value = this.getValue();

            this._emitChanged(this.#value, oldValue);
        }
    }

    setMaxLength(maxlength) {
        if (!Number.isInteger(maxlength)) return;

        this.setAttribute("maxLength", maxlength);
    }
}