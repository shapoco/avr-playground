Param($AVRDUDE_EXE, $MCU, $CABLE_NAME, $CABLE_PORT)

&"$AVRDUDE_EXE" `
    -p $MCU `
    -c $CABLE_NAME `
    -x rtsdtr=low `
    -P $CABLE_PORT `
    -U bodcfg:w:0x10:m `
    -U osccfg:w:0x01:m `
    -U syscfg0:w:0xf7:m
