package can

import (
	"github.com/brutella/can"
	"log"
)

type SocketCan struct {
	bus *can.Bus
	busHandler can.Handler
	readChan chan *can.Frame
	errChan chan error
}

func (b *SocketCan) Open() error {
	bus, err := can.NewBusForInterfaceWithName("can0")
	if err != nil {
		return err
	}

	b.bus = bus
	b.errChan = make(chan error)
	b.readChan = make(chan *can.Frame)

	go b.connectAndPublish()

	for err := range b.errChan {
		log.Fatal(err)
	}

	b.busHandler = can.NewHandler(b.handleFrame)
	b.bus.Subscribe(b.busHandler)

	//chanErr, ok := <- b.errChan
	//
	//if ok {
	//	log.Print(chanErr)
	//	return chanErr
	//}

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

func (b *SocketCan) connectAndPublish() {

	defer close(b.errChan)

	err := b.bus.ConnectAndPublish()

	if err == nil {
		return
	}

	b.errChan <- err
}

func (b *SocketCan) handleFrame(frm can.Frame) {
	b.readChan <- &frm
}
