mock "GET /effects" {
    status = 200

    headers {
        Content-Type = "application/json"
    }

    body = <<EOF
    {
        "effects":["Mii", "Estatico", "Blink", "ColorWipe", "ColorWipeInverse", "Pacifica", "Rainbow", "SnowSparkle"]
    }
    EOF
}

mock "GET /color" {
    status = 200

    headers {
        Content-Type = "application/json"
    }

    body = <<EOF
    {
        "rgb": {
            "r": "253",
            "g": "96",
            "b": "164"
        }
    }
    EOF
}

mock "GET /bright" {
    status = 200 

    headers {
        Content-Type = "application/json"
    }

    body = <<EOF
    {
        "bright": "200"
    }
    EOF
}
