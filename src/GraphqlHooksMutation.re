open GraphqlHooksTypes;

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

  [@bs.deriving abstract]
  type useMutationResponseJs = {
    loading: bool,
    cacheHit: bool,
    data: Js.Nullable.t(Js.Json.t),
    error: bool,
    graphQLErrors: Js.Nullable.t(graphqlErrors),
    fetchError: Js.Nullable.t(fetchError),
    httpError: Js.Nullable.t(httpError),
  };

  type executeMutation =
    useMutationOptions => Js.Promise.t(useMutationResponseJs);

  [@bs.module "graphql-hooks"]
  external useMutation:
    (string, useMutationOptions) => (executeMutation, useMutationResponseJs) =
    "useMutation";

  let use = (~variables=?, ~operationName=?, ()) => {
    let (executeMutation, useMutationResponseJs) =
      useMutation(
        Config.query,
        useMutationOptions(~variables?, ~operationName?, ()),
      );

    let useMutationResponseToRecord =
        (parse: Js.Json.t => 'response, state: useMutationResponseJs) => {
      let data = state->dataGet->Js.Nullable.toOption->Belt.Option.map(parse);
      let loading = state->loadingGet;
      let error = state->errorGet;
      let graphQLErrors = state->graphQLErrorsGet->Js.Nullable.toOption;
      let httpError = state->httpErrorGet->Js.Nullable.toOption;
      let fetchError = state->fetchErrorGet->Js.Nullable.toOption;
      let cacheHit = state->cacheHitGet;

      let result = {
        data,
        loading,
        error,
        graphQLErrors,
        httpError,
        fetchError,
      };

      let response =
        switch (result) {
        | {loading: true} => Loading
        | {data: Some(data)} => Data(data)
        | {error: true, graphQLErrors: Some(errors)} =>
          Error(GraphQLErrors(errors))
        | {error: true, httpError: Some(error)} => Error(HttpError(error))
        | {error: true, fetchError: Some(error)} =>
          Error(FetchError(error))
        | _ => NoData
        };

      {loading, data, cacheHit, response};
    };

    let useMutationResponse =
      React.useMemo2(
        () =>
          useMutationResponseJs |> useMutationResponseToRecord(Config.parse),
        (Config.parse, useMutationResponseJs),
      );

    let executeMutation =
      React.useCallback1(
        () => executeMutation(useMutationOptions(~variables?, ())),
        [|variables|],
      );

    (useMutationResponse, executeMutation);
  };
};