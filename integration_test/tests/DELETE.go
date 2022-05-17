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
		deleteFilePath := "tmp/delete.txt"
		deleteFileRelativePath := "../" + deleteFilePath
		if err := os.MkdirAll(filepath.Dir(deleteFileRelativePath), 0750); err != nil {
			return false, err
		}
		if _, err := os.Create(deleteFileRelativePath); err != nil {
			return false, err
		}

		clientA := tester.NewClient(&tester.Client{
			Port: "5500",
			ReqPayload: []string{
				"DELETE /" + deleteFilePath + " HTTP/1.1\r\n",
				"Host: localhost:5500\r\n",
				"User-Agent: curl/7.79.1\r\n",
				`Accept: */*` + "\r\n",
				"\r\n",
			},
			ExpectStatusCode: http.StatusNoContent,
			ExpectHeader:     nil,
			ExpectBody:       nil,
		})
		clientA.Test()

		// check file exists or deleted
		_, err := os.Stat(deleteFileRelativePath)
		switch {
		case errors.Is(err, os.ErrNotExist): // file does not exit
			return true, nil
		case err != nil:
			os.RemoveAll(filepath.Dir(deleteFileRelativePath)) // error
			return false, err
		default:
			os.RemoveAll(filepath.Dir(deleteFileRelativePath)) // file still exists
			fmt.Fprintf(os.Stderr, "file wasn't deleted")
			return false, nil
		}
	})

	testHandler("no_such_file", func() (bool, error) {
		clientA := tester.NewClient(&tester.Client{
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
			ExpectBody:       NOT_FOUND,
		})
		return clientA.Test(), nil
	})

}
