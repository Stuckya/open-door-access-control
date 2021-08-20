package can

import (
	"github.com/brutella/can"
)

type SocketCan struct {
	bus *can.Bus
	busHandler can.Handler
	readChan chan *can.Frame
}

func (b *SocketCan) Open() error {
	bus, err := can.NewBusForInterfaceWithName("can0")
	if err != nil {
		return err
	}

	b.bus = bus
	b.readChan = make(chan *can.Frame)

	b.busHandler = can.NewHandler(b.handleFrame)
	b.bus.Subscribe(b.busHandler)

	return nil
}

func (b *SocketCan) Read() chan *can.Frame {
	return b.readChan
}

func (b *SocketCan) Publish(frm can.Frame) error {
	return b.bus.Publish(frm)
}

func (b *SocketCan) Close() error {
	b.bus.Unsubscribe(b.busHandler)

	close(b.readChan)

	return b.bus.Disconnect()
}

func (b *SocketCan) ListenAndServe() error {
	return b.bus.ConnectAndPublish()
}

func (b *SocketCan) handleFrame(frm can.Frame) {
	b.readChan <- &frm
}
