import {Control} from "./base.js";

export class TextControl extends Control {
    constructor(element) {
        super(element);
    }

    get text() {
        return this.element.innerText;
    }


    setText(text) {
        this.element.innerText = text;
    }
}