import {Control} from "./base.js";

export class FrameControl extends Control {
    constructor(element) {
        super(element);
    }

    appendChild(control) {
        this.element.appendChild(control.element);
    }
}