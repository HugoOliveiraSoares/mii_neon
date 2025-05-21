mock "GET /effects" {
    status = 200

    headers {
        Content-Type = "application/json"
    }

    body = <<EOF
    {
        "effects":["Mii", "Estatico", "Blink", "ColorWipe", "Festa", "Love"]
    }
    EOF
}
