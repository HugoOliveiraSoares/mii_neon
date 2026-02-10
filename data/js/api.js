export const Api = {
    setColor: (color) =>
        fetch("/color", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(color),
        }).then((r) => r.json()),

    setStripColor: (stripIndex, color) => {
        const payload = {
            has_individual_colors: true,
            strips: [
                {
                    index: stripIndex,
                    color: {
                        r: color.red || color.r,
                        g: color.green || color.g,
                        b: color.blue || color.b,
                    },
                },
            ],
        };
        return fetch("/color", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(payload),
        }).then((r) => r.json());
    },

    fetchColor: () =>
        fetch("/color")
            .then((r) => r.json())
            .catch((error) => {
                console.error("Erro ao buscar cor:", error);
                return null;
            }),

    setBright: (bright) =>
        fetch("/bright", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(bright),
        }).then((r) => r.json()),

    fetchBright: () => fetch("/bright").then((r) => r.json()),

    fetchEffects: () => fetch("/effect").then((r) => r.json()),

    setEffect: (effect) =>
        fetch("/effect", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify(effect),
        }).then((r) => r.json()),

    scanWifi: () => fetch("/scan").then((r) => r.json()),

    saveWifi: async (formData) => {
        const r = await fetch("/savewifi", {
            method: "POST",
            body: formData,
        });
        if (!r.ok) {
            let message = "Erro desconhecido no servidor.";
            try {
                const err = await r.json();
                message = err.status || err.message || message;
            } catch {
                message = await r.text();
            }
            throw new Error(`${r.status}: ${message}`);
        }
        return r.json();
    },

    fetchWifiStatus: () => fetch("/wifistatus").then((r) => r.json()),

    firmwareUpdate: (formData) =>
        new Promise((resolve, reject) => {
            const xhr = new XMLHttpRequest();
            
            xhr.addEventListener('load', () => {
                if (xhr.status >= 200 && xhr.status < 300) {
                    try {
                        const response = JSON.parse(xhr.responseText);
                        resolve(response);
                    } catch (e) {
                        resolve(xhr.responseText);
                    }
                } else {
                    reject(new Error(`HTTP ${xhr.status}: ${xhr.statusText}`));
                }
            });
            
            xhr.addEventListener('error', () => {
                reject(new Error('Erro de rede ao enviar arquivo'));
            });
            
            xhr.open('POST', '/update');
            xhr.send(formData);
        }),

    fileSystemUpdate: (formData) =>
        new Promise((resolve, reject) => {
            const xhr = new XMLHttpRequest();
            
            xhr.addEventListener('load', () => {
                if (xhr.status >= 200 && xhr.status < 300) {
                    try {
                        const response = JSON.parse(xhr.responseText);
                        resolve(response);
                    } catch (e) {
                        resolve(xhr.responseText);
                    }
                } else {
                    reject(new Error(`HTTP ${xhr.status}: ${xhr.statusText}`));
                }
            });
            
            xhr.addEventListener('error', () => {
                reject(new Error('Erro de rede ao enviar arquivo'));
            });
            
            xhr.open('POST', '/fsupdate');
            xhr.send(formData);
        }),

};
