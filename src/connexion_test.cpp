#include "ConnexionHID.hpp"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

using namespace controldev;

int main(int argc, char**argv) {

    if(argc != 1) {
	exit(0);
    }

    ConnexionHID *c = new  ConnexionHID();
    c->init();
    connexionValues c1;
    connexionValues c2;

    while(1) {

	c->getValue(c1,c2);
        printf("Values: (%f,%f,%f),(%f,%f,%f), %s,%s \n",c1.tx,c1.ty,c1.tz,c1.rx,c1.ry,c2.rz,c1,c1.button1?"on":"off",c1.button2?"on":"off");
        usleep(4000);
    }
}
