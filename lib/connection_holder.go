package main

import "C"
import (
	"sync"

	"github.com/teonet-go/teonet"
)

// Create teonet-c connections holder
func newTeonetC() *teonetC {
	return &teonetC{m: make(map[C.int]*teonet.Teonet)}
}

var teoc = newTeonetC()

// teonetC store teonet connections in map
type teonetC struct {
	m   map[C.int]*teonet.Teonet
	key C.int
	sync.RWMutex
}

// add teonet connection and return connection key
func (t *teonetC) add(teo *teonet.Teonet) C.int {
	t.Lock()
	defer t.Unlock()
	t.key += 1
	t.m[t.key] = teo

	return t.key
}

// get teonet connection by key
func (t *teonetC) get(teoKey C.int) (teo *teonet.Teonet, c_ok C.uchar) {
	teo, ok := t.m[teoKey]
	if ok {
		c_ok = 1
	}
	return
}
