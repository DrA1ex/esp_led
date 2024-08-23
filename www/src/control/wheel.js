import {InputControlBase} from "./base.js";

export class WheelControl extends InputControlBase {
    #value = null;
    #position = 0;

    #mainPartElement;
    #fractionPartElement;

    #displayConverter = null;

    #active = false;
    #width = 0;
    #left = 0;

    margin = 20;
    limit;

    constructor(element, limit) {
        super(element)

        this.#mainPartElement = document.createElement("span");
        this.#mainPartElement.classList.add("main");
        element.appendChild(this.#mainPartElement);

        this.#fractionPartElement = document.createElement("span");
        this.#fractionPartElement.classList.add("fraction");
        element.appendChild(this.#fractionPartElement);

        this.limit = limit;

        this.addClass("input");
        this.addClass("wheel");

        this.setValue(0);

        this.element.onmousedown = this.element.ontouchstart = (e) => {
            this.#beginInteraction(e);
        }

        this.element.onmouseup = this.element.ontouchend = (e) => {
            this.#endInteraction(e);
        }

        this.element.onmousemove = this.element.ontouchmove = async (e) => {
            this.#updatePosition(e);
        };

        this.element.onmouseenter = (e) => {
            if (e.buttons === 0) this.#endInteraction(e);

        }
    }

    setValue(value) {
        this.#value = value;
        this.#position = (value / this.limit);

        let result;
        if (this.#displayConverter) {
            result = this.#displayConverter(value) ?? this.#defaultDisplayConverter(value);
        } else {
            result = this.#defaultDisplayConverter(value);
        }

        const mainPart = Array.isArray(result) ? result[0] : result;
        const fraction = Array.isArray(result) ? result[1] : "";

        this.#mainPartElement.innerText = mainPart;
        this.#fractionPartElement.innerText = fraction;

        this.element.style.setProperty("--pos", this.#position.toString());
    }

    getValue() {
        return this.#value;
    }

    setDisplayConverter(fn) {
        this.#displayConverter = fn;
    }

    #defaultDisplayConverter() {
        const percent = (this.#position * 100);
        if (percent === 0 || percent === 100) {
            return percent.toString();
        } else {
            const textValue = percent.toFixed(1).split(".");
            if (textValue[1] === "0") {
                return textValue[0];
            } else {
                return [textValue[0], `.${textValue[1]}`];
            }
        }
    }

    #beginInteraction(e) {
        this.#active = true;
        this.#width = this.element.getBoundingClientRect().width - this.margin * 2;
        this.#left = this.element.getBoundingClientRect().left + this.margin;

        e.preventDefault();
    }

    #updatePosition(e) {
        if (!this.#active) return;

        const clientX = e.touches ? e.touches[0].clientX : e.clientX;
        const pos = (clientX - this.#left) / this.#width;

        const newPos = Math.max(0, Math.min(1, pos));
        const newValue = Math.round(newPos * this.limit);

        const oldValue = this.getValue();
        this.setValue(newValue);

        this._emitChanged(newValue, oldValue);
        e.preventDefault();
    }

    #endInteraction(e) {
        if (!this.#active) return;

        this.#active = false;
        e.preventDefault();
    }
}