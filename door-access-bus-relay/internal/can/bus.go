package can

import (
	"github.com/brutella/can"
)

func CreateBus() *can.Bus {
	bus, _ := can.NewBusForInterfaceWithName("can0")

	return bus
}