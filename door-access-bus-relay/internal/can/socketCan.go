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
	// TODO: Config
	bus, err := can.NewBusForInterfaceWithName("can0")
	if err != nil {
		return err
	}

	b.bus = bus
	b.readChan = make(chan *can.Frame)

	b.busHandler = can.NewHandler(b.handleSendingFrameToChan)
	b.bus.Subscribe(b.busHandler)

	return nil
}

func (b *SocketCan) handleSendingFrameToChan(frm can.Frame) {
	b.readChan <- &frm
}

func (b *SocketCan) Read() chan *can.Frame {
	return b.readChan
}

func (b *SocketCan) PublishFrame(frm can.Frame) error {
	return b.bus.Publish(frm)
}

func (b *SocketCan) SendDataAsMaster(dataInput []byte) error {
	var data [8]uint8
	n := len(dataInput)
	copy(data[:n], dataInput[:n])

	return b.bus.Publish(can.Frame{
		// TODO: Read master ID from config/ const?
		ID:     1793,
		Length: uint8(n),
		Flags:  0,
		Res0:   0,
		Res1:   0,
		Data:   data,
	})
}

func (b *SocketCan) Close() error {
	b.bus.Unsubscribe(b.busHandler)

	close(b.readChan)

	return b.bus.Disconnect()
}

func (b *SocketCan) ListenAndServe() error {
	return b.bus.ConnectAndPublish()
}
