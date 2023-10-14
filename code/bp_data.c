#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h> // for mqtt protocol

/********************MQTT Functions************************/

const char *user = "me";
const char *pswd = "me";

int rc, mq_id = 12;
char bpressure[20] = "";
int bp_val = 0;

struct mosquitto *mosq;
void *obj;
struct mosquitto_message *msg;

void on_connect(struct mosquitto *mosq, void *obj, int rc)
{
  printf("ID: %d\n", *(int *)obj);
  if (rc)
  {
    printf("Error with result code: %d\n", rc);
    exit(-1);
  }
  mosquitto_subscribe(mosq, NULL, "bp data", 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
  sprintf(bpressure, "%s", (char*)msg->payload);
  bp_val = atoi(bpressure);
 // printf("%d ", bp_val);
}

void fetchvalue()
{
  // mosquitto_loop_forever(mosq, -1, 1);
  mosquitto_loop_start(mosq);
  // getchar();
}

void stop_fetch()
{
  mosquitto_loop_stop(mosq, 1);
}

int mqttinit()
{
  mosquitto_lib_init();
  mosq = mosquitto_new("subscribe-test", true, &mq_id);
  mosquitto_username_pw_set(mosq, user, pswd);
  mosquitto_connect_callback_set(mosq, on_connect);
  mosquitto_message_callback_set(mosq, on_message);
  rc = mosquitto_connect(mosq, "192.168.4.2", 1883, 10);
  if (rc)
  {
    printf("Could not connect to Broker with return code %d\n", rc);
    return -1;
  }
  else
  {
    printf("BP device Connected !\n");
  }
  return 0;
}

