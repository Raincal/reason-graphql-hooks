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
    error: option(combinedErrorJs),
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
      let error = state->errorGet->Belt.Option.map(combinedErrorToRecord);
      let cacheHit = state->cacheHitGet;

      let result = {data, loading, error};

      let response =
        switch (result) {
        | {loading: true} => Loading
        | {error: Some(error)} =>
          switch (error) {
          | {graphQLErrors: Some(errors)} => Error(GraphQLErrors(errors))
          | {httpError: Some(error)} => Error(HttpError(error))
          | {fetchError: Some(error)} => Error(FetchError(error))
          | _ => NoData
          }
        | {data: Some(data)} => Data(data)
        | _ => NoData
        };

      Js.log(response);

      {loading, data, cacheHit, response};
    };

    let useMutationResponse =
      React.useMemo2(
        () =>
          useMutationResponseJs |> useMutationResponseToRecord(Config.parse),
        (Config.parse, useMutationResponseJs),
      );

    let executeMutation =
      React.useMemo1(
        ((), ~variables=?, ()) =>
          executeMutation(useMutationOptions(~variables?, ())),
        [|variables|],
      );

    (useMutationResponse, executeMutation);
  };
};