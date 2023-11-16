#include <gst/gst.h>

static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data) {
    GMainLoop *loop = (GMainLoop *)data;

    switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug_info;

            gst_message_parse_error(message, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(message->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            g_main_loop_quit(loop);
            break;
        }
        case GST_MESSAGE_EOS:
            g_main_loop_quit(loop);
            break;
        default:
            break;
    }

    return TRUE;
}

int main(int argc, char *argv[]) {
    gst_init(&argc, &argv);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    // Create the pipeline
    GstElement *pipeline = gst_parse_launch("qtiqmmfsrc ! image/jpeg, width=1920, height=1080, framerate=30/1 ! jpegdec ! videoconvert ! pngenc ! filesink location=captured_image.png", NULL);

    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, bus_callback, loop);
    gst_object_unref(bus);

    // Start playing
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Run the main loop
    g_main_loop_run(loop);

    // Clean up
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    g_main_loop_unref(loop);

    return 0;
}
