import {Control} from "./base.js";

/**
 * @class
 * @extends Control<ButtonControl>
 */
export class ButtonControl extends Control {
    constructor(element) {
        super(element);

        this.addClass("button");
    }

    setLabel(label) {
        this.element.innerText = label;
    }

    setOnClick(fn) {
        this.element.onclick = (e) => fn(this, e);
    }
}