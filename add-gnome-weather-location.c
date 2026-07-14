#include <glib.h>
#include <gio/gio.h>
#include <libgweather/gweather.h>
#include <locale.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    char *end_lat = NULL;
    char *end_lon = NULL;

    setlocale(LC_ALL, "");

    if (argc != 4) {
        g_printerr(
            "Utilizare:\n"
            "  %s \"Numele localității\" LATITUDINE LONGITUDINE\n\n"
            "Exemplu:\n"
            "  %s \"Chișinău\" 47.0105 28.8638\n",
            argv[0], argv[0]
        );
        return EXIT_FAILURE;
    }

    const char *name = argv[1];

    const double latitude_deg =
        g_ascii_strtod(argv[2], &end_lat);

    const double longitude_deg =
        g_ascii_strtod(argv[3], &end_lon);

    if (end_lat == argv[2] || *end_lat != '\0' ||
        end_lon == argv[3] || *end_lon != '\0') {
        g_printerr("Coordonatele nu sunt numere valide.\n");
        return EXIT_FAILURE;
    }

    if (latitude_deg < -90.0 || latitude_deg > 90.0) {
        g_printerr("Latitudinea trebuie să fie între -90 și 90.\n");
        return EXIT_FAILURE;
    }

    if (longitude_deg < -180.0 || longitude_deg > 180.0) {
        g_printerr("Longitudinea trebuie să fie între -180 și 180.\n");
        return EXIT_FAILURE;
    }

    /* libgweather folosește radiani */
    const double latitude_rad =
        latitude_deg * G_PI / 180.0;

    const double longitude_rad =
        longitude_deg * G_PI / 180.0;

    GWeatherLocation *location =
        gweather_location_new_detached(
            name,
            NULL,
            latitude_rad,
            longitude_rad
        );

    if (location == NULL) {
        g_printerr("Nu s-a putut crea locația.\n");
        return EXIT_FAILURE;
    }

    GVariant *serialized =
        gweather_location_serialize(location);

    if (serialized == NULL) {
        g_printerr("Nu s-a putut serializa locația.\n");
        g_object_unref(location);
        return EXIT_FAILURE;
    }

    /*
     * locations are tipul „av”.
     * Fiecare element este serializarea locației împachetată în variantă.
     */
    GVariantBuilder builder;
    g_variant_builder_init(
        &builder,
        G_VARIANT_TYPE("av")
    );

    g_variant_builder_add(
        &builder,
        "v",
        serialized
    );

    GVariant *locations =
        g_variant_ref_sink(
            g_variant_builder_end(&builder)
        );

    if (!g_variant_is_of_type(
            locations,
            G_VARIANT_TYPE("av"))) {
        g_printerr(
            "Eroare internă: rezultatul nu are tipul av.\n"
        );

        g_variant_unref(locations);
        g_object_unref(location);
        return EXIT_FAILURE;
    }

    GSettings *settings =
        g_settings_new("org.gnome.Weather");

    gboolean success =
        g_settings_set_value(
            settings,
            "locations",
            locations
        );

    if (!success) {
        g_printerr(
            "Scrierea cheii GSettings a eșuat.\n"
        );

        g_object_unref(settings);
        g_variant_unref(locations);
        g_object_unref(location);
        return EXIT_FAILURE;
    }

    g_settings_sync();

    gchar *text =
        g_variant_print(locations, TRUE);

    g_print(
        "Locația a fost adăugată:\n"
        "  %s — %.6f, %.6f grade\n"
        "  %.12f, %.12f radiani\n\n"
        "GSettings:\n%s\n",
        name,
        latitude_deg,
        longitude_deg,
        latitude_rad,
        longitude_rad,
        text
    );

    g_free(text);
    g_object_unref(settings);
    g_variant_unref(locations);
    g_object_unref(location);

    return EXIT_SUCCESS;
}
