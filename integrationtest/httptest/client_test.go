package httptest

import (
	"bytes"
	"io"
	"log"
	"net"
	"net/http"
	"testing"
	"time"

	"github.com/google/go-cmp/cmp"
)

type TestConn struct {
	bytes.Buffer
}

// Connインターフェースを満たすためにダミーのメソッドはやしてる
func (tc TestConn) Close() error                       { return nil }
func (tc TestConn) LocalAddr() net.Addr                { return nil }
func (tc TestConn) RemoteAddr() net.Addr               { return nil }
func (tc TestConn) SetDeadline(t time.Time) error      { return nil }
func (tc TestConn) SetReadDeadline(t time.Time) error  { return nil }
func (tc TestConn) SetWriteDeadline(t time.Time) error { return nil }

func TestClient_SendRequestAll(t *testing.T) {
	type fields struct {
		Port           string
		Request        string
		Conn           net.Conn
		Close          bool
		GotResponse    *http.Response
		ReponseChecker ReponseChecker
		sendCnt        int
		sendLog        []string
	}
	tests := []struct {
		name   string
		fields fields
	}{
		{
			name: "全て送信できているか",
			fields: fields{
				Request: "request",
				Conn:    new(TestConn),
			},
		},
	}
	for _, tt := range tests {
		tt := tt
		t.Run(tt.name, func(t *testing.T) {
			c := &Client{
				Port:           tt.fields.Port,
				Request:        tt.fields.Request,
				Conn:           tt.fields.Conn,
				Close:          tt.fields.Close,
				GotResponse:    tt.fields.GotResponse,
				ReponseChecker: tt.fields.ReponseChecker,
				sendCnt:        tt.fields.sendCnt,
				sendLog:        tt.fields.sendLog,
			}
			c.SendRequestAll()
			sendRequest, err := io.ReadAll(c.Conn)
			if err != nil {
				log.Fatal(err)
			}
			if diff := cmp.Diff(c.Request, string(sendRequest)); diff != "" {
				t.Errorf("reqeust mismatch (-want +got):\n%s", diff)
			}
			if diff := cmp.Diff(len(c.Request), c.sendCnt); diff != "" {
				t.Errorf("send cnount mismatch (-want +got):\n%s", diff)
			}
		})
	}
}
