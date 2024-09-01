import {InputControlBase} from "./base.js";

export class SelectControl extends InputControlBase {
    #selected = null;
    #select;

    options = [];

    constructor(element) {
        super(element);

        this.addClass("input");

        this.#select = document.createElement("select")
        this.element.appendChild(this.#select);

        this.#select.onchange = this.#onChangeHandler.bind(this);
        this.element.onclick = this.#onClick.bind(this);
    }

    get selected() {
        return this.#selected?.key ?? null;
    }

    #onChangeHandler() {
        const oldValue = this.selected;
        this.select(this.#select.value);

        this._emitChanged(this.selected, oldValue);
    }

    /**
     * @param {Array<{key: string, label: string}|*>} options
     */
    setOptions(options) {
        const selectedKey = this.selected;
        this.#selected = null;
        this.options = [];
        this.#select.innerHTML = "";

        for (let option of options) {
            const e = document.createElement("option");
            const entry = {e};

            if (option instanceof Object) {
                entry.key = option.key
                entry.strKey = option.strKey ?? option.key.toString();
                entry.label = option.label ?? option.key.toString();
            } else {
                entry.key = option
                entry.strKey = option.toString();
                entry.label = option.toString();
            }

            e.value = entry.strKey;
            e.text = entry.label;

            this.#select.appendChild(e);
            this.options.push(entry);
        }

        this.select(selectedKey);
    }

    updateOption(key, label) {
        const option = this.options.find(o => o.key === key || o.strKey === key);
        if (!option) return;

        option.label = label;
        option.e.innerText = label;
    }

    select(key) {
        if (this.#selected) {
            this.#selected.e.removeAttribute("selected");
        }

        const option = this.options.find(o => o.key === key || o.strKey === key);
        if (option) {
            option.e.setAttribute("selected", "");
            this.#selected = option;
        } else {
            this.#selected = null;
        }

        this.#select.value = this.selected;
    }

    setValue(value) {
        this.select(value);
    }

    getValue() {
        return this.selected;
    }

    #onClick(e) {
        const selectRect = this.#select.getBoundingClientRect()

        let direction = 0;
        if (e.clientX < selectRect.left) {
            direction = -1;
        } else if (e.clientX > selectRect.right) {
            direction = 1;
        }

        if (direction === 0) return;

        const value = Number.parseInt(this.selected);
        if (!Number.isFinite(value)) return;

        let index = this.options.findIndex(e => e.key === value);
        if (index >= 0) {
            index = (this.options.length + index + direction) % this.options.length;

            const oldValue = this.selected;
            const newValue = this.options[index].key;

            if (oldValue !== newValue) {
                this.select(newValue);
                this._emitChanged(newValue, oldValue);
            }
        }

        e.preventDefault();
    }
}