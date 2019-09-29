/*
   Infrared sensor support for generic 24 key RGB remote
*/

#ifndef WLED_DISABLE_KNX

void knxInit()
{
  knx.start();
  address_t switch_ga = knx.GA_to_address(1, 3, 2); // the KNX group address to send ON/OFF command OR a byte object (dimming)
  address_t status_ga = knx.GA_to_address(1, 4, 2); // the KNX group address that send it's boolean status
  address_t scene_ga  = knx.GA_to_address(1, 6, 0); // the KNX group address to receive a scene object 
  address_t device_pa = knx.PA_to_address(0, 1, 3); // the KNX adresse of the object (anything unique)

  
  knx.physical_address_set(device_pa);

  callback_id_t knx_switch_id = knx.callback_register("switch", knx_switch);
  callback_id_t knx_status_id = knx.callback_register("status", knx_status);
  callback_id_t knx_scene_id = knx.callback_register("scene", knx_scene);

  knx.callback_assign(knx_switch_id, switch_ga);
  knx.callback_assign(knx_status_id, status_ga);
  knx.callback_assign(knx_scene_id, scene_ga);
}

void handleKnx()
{
  knx.loop();
}

void knx_switch(message_t const &msg, void *arg)
{
  switch (msg.ct)
  {
    case KNX_CT_WRITE:
      DEBUG_PRINTLN("knx received switch");
      if (knx.data_to_1byte_int(msg.data)!=0)
      {
        DEBUG_PRINTLN("knx switching on if bri=0");
        if ( bri == 0 )
        {
          bri = briLast;
          colorUpdated(5);
        }
      }
      else
      {
        DEBUG_PRINTLN("knx switching off if bri=!0");
        if ( bri != 0 )
        {
          briLast = bri;
          bri = 0;
          colorUpdated(5);
        }
      }
      break;
  }
}

void knx_status(message_t const &msg, void *arg)
{
  switch (msg.ct)
  {
    case KNX_CT_READ:
      // Answer with saved data
      knx.answer_1bit(msg.received_on, (bri!=0) );
      break;
  }
}

void knx_scene(message_t const &msg, void *arg)
{
  switch (msg.ct)
  {
    case KNX_CT_WRITE:
      // Save received data
      DEBUG_PRINT("knx received scene : ");
      DEBUG_PRINTLN(knx.data_to_1byte_int(msg.data));
      applyPreset(knx.data_to_1byte_int(msg.data), true, true, true); 
      if (!turnOnAtBoot && knx.data_to_1byte_int(msg.data)==0) {
        if ( bri != 0 )
        {
          briLast = bri;
          bri = 0;
        }
      }
      colorUpdated(5);
      break;
  }
}
#else
void knxInit() {}
void handleKnx() {}
#endif
