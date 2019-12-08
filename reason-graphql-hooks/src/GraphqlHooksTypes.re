module type Config = {
  type t;
  let query: string;
  let parse: Js.Json.t => t;
};

type sourceLocation = {
  line: int,
  column: int,
};

type locations = array(sourceLocation);

type operation = {
  query: string,
  variables: Js.Nullable.t(Js.Json.t),
  operationName: Js.Nullable.t(string),
};

type graphQLError = {
  locations,
  path: array(string),
  code: int,
  message: string,
  requestId: string,
};

type httpError = {
  status: int,
  statusText: string,
  body: string,
};

type error =
  | GraphQLErrors(array(graphQLError))
  | HttpError(httpError)
  | FetchError(Js.Exn.t);

type combinedErrorJs = {
  fetchError: Js.Nullable.t(Js.Exn.t),
  graphQLErrors: Js.Nullable.t(array(graphQLError)),
  httpError: Js.Nullable.t(httpError),
};

type combinedError = {
  fetchError: option(Js.Exn.t),
  graphQLErrors: option(array(graphQLError)),
  httpError: option(httpError),
};

let combinedErrorToRecord = (err: combinedErrorJs): combinedError => {
  {
    fetchError: err.fetchError->Js.Nullable.toOption,
    graphQLErrors: err.graphQLErrors->Js.Nullable.toOption,
    httpError: err.httpError->Js.Nullable.toOption,
  };
};

type response('a) =
  | Data('a)
  | Error(error)
  | Loading
  | NoData;

type result('a) = {
  data: option('a),
  loading: bool,
  error: option(combinedError),
};

type hooksResponse('a) = {
  loading: bool,
  data: option('a),
  cacheHit: bool,
  response: response('a),
};

type parse('a) = Js.Json.t => 'a;
type query = string;
type composeVariables('returnType, 'hookReturnType) =
  (Js.Json.t => 'returnType) => 'hookReturnType;

type graphqlDefinition('data, 'returnType, 'hookReturnType) = (
  parse('data),
  query,
  composeVariables('returnType, 'hookReturnType),
);