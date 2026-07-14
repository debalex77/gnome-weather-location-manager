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
            "Usage:\n"
            "  %s \"Location name\" LATITUDE LONGITUDE\n\n"
            "Example:\n"
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
        g_printerr("The coordinates are not valid numbers.\n");
        return EXIT_FAILURE;
    }

    if (latitude_deg < -90.0 || latitude_deg > 90.0) {
        g_printerr("Latitude must be between -90 and 90 degrees.\n");
        return EXIT_FAILURE;
    }

    if (longitude_deg < -180.0 || longitude_deg > 180.0) {
        g_printerr("Longitude must be between -180 and 180 degrees.\n");
        return EXIT_FAILURE;
    }

    /* libgweather uses radians */
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
        g_printerr("Failed to create the location.\n");
        return EXIT_FAILURE;
    }

    GVariant *serialized =
        gweather_location_serialize(location);

    if (serialized == NULL) {
        g_printerr("Failed to serialize the location.\n");
        g_object_unref(location);
        return EXIT_FAILURE;
    }

    /*
     * The "locations" key has the GVariant type "av".
     * Each element contains a serialized location wrapped in a variant.
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
            "Internal error: the resulting GVariant does not have type 'av'.\n"
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
            "Failed to write the GSettings key.\n"
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
        "Location added successfully:\n"
        "  %s — %.6f, %.6f degrees\n"
        "  %.12f, %.12f radians\n\n"
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
