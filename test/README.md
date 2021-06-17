## How to run test

#### Description

After the program is built and running, use it to test the APIs.

#### Tools

The test is performed by **Newman**, which is the CLI version of **Postman**.
To download **Newman**, use the command `npm install -g newman`

### Run tests

To run test, use the command `newman run newman-collection.json`. The default port to test is `9090`.
If the application is running on other port, specify it by appending `env-var "port={APP PORT}"`.