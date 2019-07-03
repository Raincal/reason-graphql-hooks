open GraphqlHooksTypes;

type result('a) =
  | Data('a)
  | Error(error)
  | NoData;

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
  type useMutationOptions = {
    [@bs.optional]
    variables: Js.Json.t,
    [@bs.optional]
    operationName: string,
    [@bs.optional]
    fetchOptionsOverrides: Fetch.requestInit,
  };

  type useMutationResponseJs = {
    .
    "data": Js.Nullable.t(Js.Json.t),
    "loading": bool,
    "error": bool,
    "graphQLErrors": Js.Nullable.t(graphqlErrors),
    "fetchError": Js.Nullable.t(fetchError),
    "httpError": Js.Nullable.t(httpError),
  };

  type executeMutation =
    (. useMutationOptions) => Js.Promise.t(useMutationResponseJs);

  [@bs.module "graphql-hooks"]
  external useMutation:
    (string, useMutationOptions) => (executeMutation, useMutationResponseJs) =
    "";

  let use = (~variables=?, ~operationName=?, ()) => {
    let (executeMutation, useMutationResponseJs) =
      useMutation(
        Config.query,
        useMutationOptions(~variables?, ~operationName?, ()),
      );

    let mutate =
      React.useMemo1(
        ((), ~variables=?, ()) =>
          executeMutation(. useMutationOptions(~variables?, ())),
        [|variables|],
      );

    let full = {
      data:
        useMutationResponseJs##data
        ->Js.Nullable.toOption
        ->Belt.Option.map(Config.parse),
      loading: useMutationResponseJs##loading,
      error: useMutationResponseJs##error,
      graphQLErrors:
        useMutationResponseJs##graphQLErrors->Js.Nullable.toOption,
      httpError: useMutationResponseJs##httpError->Js.Nullable.toOption,
      fetchError: useMutationResponseJs##fetchError->Js.Nullable.toOption,
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