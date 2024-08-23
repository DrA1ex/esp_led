export class FileAsyncReader {
    constructor(file) {
        this.file = file
        this.size = file.size
    }

    async* [Symbol.asyncIterator]() {
        const reader = this.file.stream().getReader()

        while (true) {
            const chunk = await reader.read()
            if (chunk.done) {
                break
            }

            yield chunk.value
        }
    }

    async load() {
        const size = this.file.size
        const data = new Uint8Array(size)

        let offset = 0
        for await (const chunk of this) {
            data.set(chunk, offset)
            offset += chunk.length
        }

        return data.buffer
    }
}
