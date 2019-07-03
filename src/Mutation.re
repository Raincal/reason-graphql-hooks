open GraphqlHooksTypes;

/* Result that is return by the hook */
type result('a) =
  | Data('a)
  | Error(error)
  | NoData;

/* Result that is return by the hook */
type controlledResult('a) = {
  data: option('a),
  loading: bool,
  error: bool,
  graphQLErrors: option(graphqlErrors),
  httpError: option(httpError),
  fetchError: option(fetchError),
};

type controledVariantResult('a) =
  | Data('a)
  | Error(error)
  | Loading
  | NoData;

module Make = (Config: Config) => {
  [@bs.deriving abstract]
  type options = {
    [@bs.optional]
    variables: Js.Json.t,
    [@bs.optional]
    operationName: string,
    [@bs.optional]
    fetchOptionsOverrides: Fetch.requestInit,
  };

  type jsResult = {
    .
    "data": Js.Nullable.t(Js.Json.t),
    "loading": bool,
    "error": bool,
    "graphQLErrors": Js.Nullable.t(graphqlErrors),
    "fetchError": Js.Nullable.t(fetchError),
    "httpError": Js.Nullable.t(httpError),
  };

  type jsMutate = (. options) => Js.Promise.t(jsResult);

  [@bs.module "graphql-hooks"]
  external useMutation: (string, options) => (jsMutate, jsResult) = "";

  let use = (~variables=?, ~operationName=?, ()) => {
    let (jsMutate, jsResult) =
      useMutation(Config.query, options(~variables?, ~operationName?, ()));

    let mutate =
      React.useMemo1(
        ((), ~variables=?, ()) =>
          jsMutate(. options(~variables?, ()))
          |> Js.Promise.then_(jsResult =>
               (
                 switch (
                   Js.Nullable.toOption(jsResult##data),
                   jsResult##error,
                 ) {
                 | (Some(data), _) => Data(Config.parse(data))
                 //  | (None, true) => Error()
                 | (None, _) => NoData
                 }
               )
               |> Js.Promise.resolve
             ),
        [|variables|],
      );

    let full = {
      data:
        jsResult##data->Js.Nullable.toOption->Belt.Option.map(Config.parse),
      loading: jsResult##loading,
      error: jsResult##error,
      graphQLErrors: jsResult##graphQLErrors->Js.Nullable.toOption,
      httpError: jsResult##httpError->Js.Nullable.toOption,
      fetchError: jsResult##fetchError->Js.Nullable.toOption,
    };

    let simple =
      switch (full) {
      | {loading: true} => Loading
      | {data: Some(data)} => Data(data)
      | {error: true, graphQLErrors: Some(errors)} =>
        Error(GraphQLErrors(errors))
      | {error: true, httpError: Some(error)} => Error(HttpError(error))
      | {error: true, fetchError: Some(error)} => Error(FetchError(error))
      | _ => NoData
      };

    (mutate, simple, full);
  };
};