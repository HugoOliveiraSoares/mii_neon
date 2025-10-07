export const Api = {
  setColor: (color) =>
    fetch("/color", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify(color),
    }).then((r) => r.json()),

  fetchColor: () =>
    fetch("/color")
      .then((r) => r.json())
      .then((data) => {
        if (data.rgb && typeof data.rgb.r !== "undefined") {
          return data.rgb;
        } else {
          throw new Error("Formato de dados inválido" + JSON.stringify(data));
        }
      })
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

  fetchEffects: () => fetch("/effects").then((r) => r.json()),

  setEffect: (effect) =>
    fetch("/effects", {
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
    fetch("/update", {
      method: "POST",
      body: formData,
    }).then((r) => r.json()),
};
