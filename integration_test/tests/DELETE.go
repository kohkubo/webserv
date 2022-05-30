package tests

import (
	"errors"
	"fmt"
	"integration_test/tester"
	"net/http"
	"os"
	"path/filepath"
)

func TestDELETE() {
	fmt.Println("DELETE test")

	testHandler("simple", func() (bool, error) {
		// setup file to delete
		deleteFilePath := "/tmp/delete.txt"                         // httpリクエストで指定するターゲットURI
		rootRelativePath := "../html"                               // configで指定されているrootへの(integration_testからの)相対パス
		deleteFileRelativePath := rootRelativePath + deleteFilePath // ターゲットURIへの相対パス
		if err := os.MkdirAll(filepath.Dir(deleteFileRelativePath), 0750); err != nil {
			return false, err
		}
		if _, err := os.Create(deleteFileRelativePath); err != nil {
			return false, err
		}
		defer os.RemoveAll(filepath.Dir(deleteFileRelativePath))

		clientA, err := tester.NewClient(&tester.Client{
			Port: "5500",
			ReqPayload: []string{
				"DELETE " + deleteFilePath + " HTTP/1.1\r\n",
				"Host: localhost:5500\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusNoContent,
			ExpectHeader:     nil,
			ExpectBody:       nil,
		})
		if err != nil {
			return false, err
		}
		if ok, err := clientA.Test(); err != nil {
			return false, err
		} else if !ok {
			return false, nil
		}

		// check file exists or deleted
		_, err = os.Stat(deleteFileRelativePath)
		switch {
		case errors.Is(err, os.ErrNotExist): // file does not exit
			return true, nil
		case err != nil: // error
			return false, err
		default: // file still exists
			fmt.Fprintf(os.Stderr, "file wasn't deleted")
			return false, nil
		}
	})

	testHandler("no_such_file", func() (bool, error) {
		clientA, err := tester.NewClient(&tester.Client{
			Port: "5500",
			ReqPayload: []string{
				"DELETE /no_such_file HTTP/1.1\r\n",
				"Host: localhost:5500\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusNotFound,
			ExpectHeader:     nil,
			ExpectBody:       content_404,
		})
		if err != nil {
			return false, err
		}
		return clientA.Test()
	})

}
